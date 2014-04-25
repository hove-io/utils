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

#include "timer.h"

Timer::Timer() : start(std::chrono::system_clock::now()), print_at_destruction(false) {}

Timer::Timer(const std::string &name, bool print_at_destruction) : start(std::chrono::system_clock::now()), name(name), print_at_destruction(print_at_destruction){}

Timer::~Timer() {
    if(this->print_at_destruction)
        std::cout << *this << std::endl;
}

int Timer::ms() const {
    auto delta = std::chrono::system_clock::now() - this->start;
    return std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
}

void Timer::reset() {
    start = std::chrono::system_clock::now();
}

std::ostream & operator<<(std::ostream & os, const Timer & timer){
    os << "Timer " << timer.name << " ";
    int ms = timer.ms();

    if(ms < 10000)
        os << ms << " ms";
    else
        os << (ms/1000) << " s";

    return os;
}

