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

#include <boost/date_time/posix_time/posix_time.hpp>
#include "utils/deadline.h"

namespace navitia{

    Deadline::Deadline() {}
    Deadline::Deadline(const boost::posix_time::ptime& deadline) : deadline(deadline) {}
    void Deadline::set(const boost::posix_time::ptime& deadline){
        this->deadline = deadline;
    }

    const boost::optional<boost::posix_time::ptime>& Deadline::get() const{
        return this->deadline;
    }

    bool Deadline::expired(const boost::posix_time::ptime& now) const{
        if(!deadline) {
            return false;
        }
        return now > deadline;
    }

    void Deadline::check(const boost::posix_time::ptime& now) const{
        if(this->expired(now)){
            throw DeadlineExpired("deadline expired at " + boost::posix_time::to_iso_string(*deadline));
        }
    }

}
