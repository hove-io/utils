/* Copyright © 2001-2018, Canal TP and/or its affiliates. All rights reserved.

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

#include <boost/date_time/posix_time/posix_time.hpp>

namespace navitia {

class ptime: public boost::posix_time::ptime
{
public:
    // Forward constructors
    ptime(boost::gregorian::date d,time_duration_type td) : boost::posix_time::ptime(d,td)
    {}
    explicit ptime(boost::gregorian::date d) : boost::posix_time::ptime(d)
    {}
    ptime(const time_rep_type& rhs): boost::posix_time::ptime(rhs)
    {}
    ptime(const boost::date_time::special_values sv) : boost::posix_time::ptime(sv)
    {}

    // Add (copy-)constructor from ancestor
    ptime(const boost::posix_time::ptime& rhs): boost::posix_time::ptime(rhs)
    {}

#if !defined(DATE_TIME_NO_DEFAULT_CONSTRUCTOR)
    // Add noexcept default constructor
    // Cause: Clang requires std::atomic content to have noexcept default constructor
    ptime() noexcept: boost::posix_time::ptime()
    {}
#endif // DATE_TIME_NO_DEFAULT_CONSTRUCTOR

};

} // namespace navitia
