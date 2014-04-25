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

#pragma once
#include <postgresql/libpq-fe.h>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>

struct LotusException : public std::exception {
    std::string message;
    LotusException(const std::string & message) : message(message) {}
    virtual const char* what() const throw() {
        return message.c_str();
    }
    virtual ~LotusException() throw (){}
};

struct Lotus {
    std::string delimiter;
    std::string null_value;
    PGconn* connection;

    Lotus(const std::string & connection_string) : delimiter(";"), null_value("NULL") {
        this->connection = PQconnectdb(connection_string.c_str());
        if(PQstatus(this->connection) != CONNECTION_OK){
            throw LotusException(std::string("Impossible de se connecter : ") +  PQerrorMessage(this->connection));
        }
    }

    void start_transaction() {
        PQclear(this->exec("BEGIN", "Créer la transaction"));
    }

    void rollback() {
        PQclear(this->exec("ROLLBACK", "Rollback de la transaction"));
    }

    void commit() {
        PQclear(this->exec("COMMIT", "Fermer la transaction"));
    }

    /// Attention, c’est à l’appelant de nettoyer le résultat !
    PGresult* exec(const std::string & request, const std::string & error_message = std::string(), int expected_code = PGRES_COMMAND_OK) {
        PGresult *res = PQexec(this->connection, request.c_str());
        if (PQresultStatus(res) != expected_code) {
            std::string message = "Impossible d’exécuter la requête : " + error_message + " " +  PQresultErrorMessage(res);
            PQclear(res);
            throw LotusException(message);
        }
        return res;
    }


    void prepare_bulk_insert(const std::string & table, const std::vector<std::string> & columns) {
        std::string request = "COPY " + table + "(" +  boost::algorithm::join(columns, ",") + ")  FROM STDIN WITH (FORMAT CSV, DELIMITER '" + delimiter + "', NULL '" + null_value + "', QUOTE '\"')";
        PQclear(this->exec(request, "Préparer le bulk insert",  PGRES_COPY_IN));
    }


    void insert(std::vector<std::string> elements) { // On copie le tableau pour le modifier
        for(std::string & element : elements){
            if(element != null_value){
                element = "\"" + boost::algorithm::replace_all_copy(element, "\"", "\"\"") + "\"";
            }
        }

        std::string line = boost::algorithm::join(elements, this->delimiter) + "\n";
        int result_code = PQputCopyData(this->connection, line.c_str(),  line.size());
        if(result_code != 1){
            throw LotusException(std::string("Impossible d’ajouter une ligne en bulk insert ") + PQerrorMessage(this->connection));
        }
    }

    void finish_bulk_insert() {
        int result_code =  PQputCopyEnd(this->connection, NULL);
        if(result_code != 1){
            throw LotusException(std::string("Impossible de finir le bulk insert ") + PQerrorMessage(this->connection));
        }

        PGresult *res = NULL;
        do{
            PQclear(res);
            res = NULL;
            res = PQgetResult(this->connection);
            if(res != NULL && PQresultStatus(res) != PGRES_COMMAND_OK){
                throw LotusException(std::string("Impossible de finir le bulk insert, dans la boucle de fin ") + PQerrorMessage(this->connection));
            }
        } while(res != NULL);

    }

    void close_connection() {
        PQfinish(this->connection);
    }

    ~Lotus() {
        this->close_connection();
    }
};
