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

#include <string>
#include <iostream>
#include <tuple>
#include <sys/times.h>

/// Small tool to time something and print these informations
struct Timer {
private:
    struct tms tms_start;
    clock_t real_start;
    std::string header;
    bool print_at_destruction;

public:
    Timer();
    Timer(const std::string& header, bool print_at_destruction = true);
    ~Timer();
    int ms() const;

    // Returns real, user and system time in seconds
    std::tuple<double, double, double> get_real_user_sys() const;

    /// Reset the start to now
    void reset();

    /// Print enlapsed time
    friend std::ostream& operator<<(std::ostream& os, const Timer& timer);
};

template <class Function, class... Args>
inline auto time_it(const std::string& header, Function&& f, Args&&... args)
    -> decltype(f(std::forward<Args>(args)...)) {
    Timer t(header);
    return f(std::forward<Args>(args)...);
}
