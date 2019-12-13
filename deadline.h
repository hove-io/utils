/* Copyright © 2001-2019, Canal TP and/or its affiliates. All rights reserved.

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
#include "utils/exception.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>

namespace navitia {

class DeadlineExpired : public recoverable_exception {
    using recoverable_exception::recoverable_exception;

public:
    DeadlineExpired(const DeadlineExpired& o) = default;
    virtual ~DeadlineExpired();
};

class Deadline {
    boost::optional<boost::posix_time::ptime> deadline;

public:
    Deadline();
    Deadline(const boost::posix_time::ptime& deadline);
    void set(const boost::posix_time::ptime& deadline);
    const boost::optional<boost::posix_time::ptime>& get() const;

    /**
     * return true if the deadline has expired
     */
    bool expired(const boost::posix_time::ptime& now = boost::posix_time::microsec_clock::universal_time()) const;

    /**
     * throw DeadlineExpired if the deadline has expired
     */
    void check(const boost::posix_time::ptime& now = boost::posix_time::microsec_clock::universal_time()) const;
};
}  // namespace navitia
