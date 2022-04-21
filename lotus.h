/* Copyright © 2001-2014, Hove and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Hove (www.hove.com).
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

// for PGRES_COMMAND_OK
#include <postgresql/libpq-fe.h>

#include <exception>
#include <string>
#include <vector>

struct LotusException : public std::exception {
    std::string const message;
    LotusException(std::string message) : message(std::move(message)) {}
    const char* what() const noexcept override;
};

struct Lotus {
    std::string const delimiter;
    std::string const null_value;
    PGconn* const connection;

    Lotus(const std::string& connection_string);
    Lotus() = delete;
    Lotus(const Lotus&) = delete;
    Lotus& operator=(const Lotus&) = delete;
    ~Lotus();

    void start_transaction();
    void rollback();
    void commit();
    void exec(const std::string& request,
              const std::string& error_message = std::string(),
              int expected_code = PGRES_COMMAND_OK);
    void prepare_bulk_insert(const std::string& table, const std::vector<std::string>& columns);
    void insert(std::vector<std::string> elements);
    void finish_bulk_insert();
    void close_connection();

    static std::string make_upsert_string(const std::string& table,
                                          const std::vector<std::pair<std::string, std::string>>& key_values);
};
