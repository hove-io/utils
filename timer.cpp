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

#include <boost/lexical_cast.hpp>
#include <unistd.h>
#include <utility>

std::tuple<double, double, double> Timer::get_real_user_sys() const {
    const long ticks_ps = sysconf(_SC_CLK_TCK);
    struct tms tms_end {};
    const clock_t real_end = times(&tms_end);
    return std::make_tuple((boost::lexical_cast<double>(real_end) - real_start) / ticks_ps,
                           (boost::lexical_cast<double>(tms_end.tms_utime) - tms_start.tms_utime) / ticks_ps,
                           (boost::lexical_cast<double>(tms_end.tms_stime) - tms_start.tms_stime) / ticks_ps);
}

Timer::Timer() : print_at_destruction(false) {
    reset();
}

Timer::Timer(std::string h, bool p) : header(std::move(h)), print_at_destruction(p) {
    reset();
}

Timer::~Timer() {
    if (this->print_at_destruction) {
        std::cout << *this << std::endl;
    }
}

int Timer::ms() const {
    return int(std::get<0>(get_real_user_sys()) * 1000.);
}

void Timer::reset() {
    real_start = times(&tms_start);
}

std::ostream& operator<<(std::ostream& os, const Timer& timer) {
    auto rus = timer.get_real_user_sys();
    return os << timer.header << "real = " << std::get<0>(rus) << "s, user = " << std::get<1>(rus)
              << "s, sys = " << std::get<2>(rus) << "s";
}
