/* Copyright © 2001-2014, Canal TP and/or its affiliates. All rights reserved.
  
This file is part of Navitia,
    the software to build cool stuff with public transport.
 
Hope you'll enjoy and contribute to this project,
    powered by Canal TP (www.canaltp.fr).
Help us simplify mobility and open public transport:
    a non ending quest to the responsive locomotion way of traveling!
  
LICENCE: This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
   
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.
   
You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
  
Stay tuned using
twitter @navitia 
IRC #navitia on freenode
https://groups.google.com/d/forum/navitia
www.navitia.io
*/

#include "lotus.h"

#include <postgresql/libpq-fe.h>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>

const char* LotusException::what() const noexcept {
    return message.c_str();
}

Lotus::Lotus(const std::string & connection_string):
    delimiter(";"),
    null_value("__NULL"),
    connection(PQconnectdb(connection_string.c_str()))
{
    if (PQstatus(this->connection) != CONNECTION_OK) {
        throw LotusException(std::string("Failed to connect: ")
                             + PQerrorMessage(this->connection));
    }
}

void Lotus::start_transaction() {
    this->exec("BEGIN", "Begin transaction");
}

void Lotus::rollback() {
    this->exec("ROLLBACK", "Rollback transaction");
}

void Lotus::commit() {
    this->exec("COMMIT", "Commit transaction");
}

void Lotus::exec(const std::string& request, const std::string& error_message, int expected_code) {
    PGresult *res = PQexec(this->connection, request.c_str());
    if (PQresultStatus(res) != expected_code) {
        std::string message = "Request failed: " + error_message + " " +  PQresultErrorMessage(res);
        PQclear(res);
        throw LotusException(message);
    }
    PQclear(res);
}


void Lotus::prepare_bulk_insert(const std::string& table, const std::vector<std::string>& columns) {
    std::string request = "COPY " + table + "(" + boost::algorithm::join(columns, ",")
        + ")  FROM STDIN WITH (FORMAT CSV, DELIMITER '" + delimiter + "', NULL '" + "NULL"
        + "', QUOTE '\"')";
    this->exec(request, "Prepare bulk insert",  PGRES_COPY_IN);
}


void Lotus::insert(std::vector<std::string> elements) {
    for(std::string & element : elements){
        if(element != null_value){
            element = "\"" + boost::algorithm::replace_all_copy(element, "\"", "\"\"") + "\"";
        } else {
            element = "NULL";
        }
    }

    std::string line = boost::algorithm::join(elements, this->delimiter) + "\n";
    int result_code = PQputCopyData(this->connection, line.c_str(),  int(line.size()));
    if(result_code != 1){
        throw LotusException(std::string("Failed to insert an entry in bulk insert ")
                             + PQerrorMessage(this->connection));
    }
}

void Lotus::finish_bulk_insert() {
    int result_code =  PQputCopyEnd(this->connection, NULL);
    if(result_code != 1){
        throw LotusException(std::string("finish bulk insert failed: ")
                             + PQerrorMessage(this->connection));
    }

    PGresult *res = NULL;
    do{
        PQclear(res);
        res = NULL;
        res = PQgetResult(this->connection);
        if(res != NULL && PQresultStatus(res) != PGRES_COMMAND_OK){
            PQclear(res);
            throw LotusException(std::string("failed to finish bulk insert in final loop: ")
                                 + PQerrorMessage(this->connection));
        }
    } while(res != NULL);

}

void Lotus::close_connection() {
    PQfinish(this->connection);
}

std::string Lotus::make_upsert_string(const std::string& table,
        const std::vector<std::pair<std::string, std::string>>& key_values) {
    std::vector<std::string> key_value_updates;
    key_value_updates.resize(key_values.size());
    std::transform(key_values.begin(), key_values.end(), key_value_updates.begin(),
            [](std::pair<std::string, std::string> kv) {
            return kv.first + "='"+ kv.second + "'";
            });
    std::string update_query = "update " + table + " set " +
        boost::algorithm::join(key_value_updates, ",");


    std::vector<std::string> keys_inserts, values_inserts;
    keys_inserts.resize(key_values.size());
    values_inserts.resize(key_values.size());
    std::transform(key_values.begin(), key_values.end(), values_inserts.begin(),
            [](std::pair<std::string, std::string> kv) {
            return "'"+ kv.second + "'";
            });
    std::transform(key_values.begin(), key_values.end(), keys_inserts.begin(),
            [](std::pair<std::string, std::string> kv) {
            return kv.first;
            });

    std::string insert_query = "insert into " + table + " ("+
        boost::algorithm::join(keys_inserts, ",") + ") select " +
        boost::algorithm::join(values_inserts, ",");

    return "WITH upsert AS "
    "(" + update_query + " RETURNING *) "
    + insert_query + " WHERE NOT EXISTS (SELECT * FROM upsert);";
}

Lotus::~Lotus() {
    this->close_connection();
}
