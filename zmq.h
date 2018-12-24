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
/**
 * this file provide a compatibility layer between zmq 2 and zmq 3
 * by providing a API similar to zmq 2
 */

#pragma once
#include <zmq.hpp>
#include <stack>

void z_send(zmq::socket_t& socket, const std::string& str, int flags=0);
void z_send(zmq::socket_t& socket, zmq::message_t& msg, int flags=0);
std::string z_recv(zmq::socket_t& socket);

class LoadBalancer{
    std::stack<std::string> avalailable_worker;
    zmq::socket_t clients;
    zmq::socket_t workers;
    public:
    LoadBalancer(zmq::context_t& context);
    void bind(const std::string& clients_socket_path, const std::string& workers_socket_path);
    void run();
};
