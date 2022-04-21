/* Copyright © 2001-2015, Hove and/or its affiliates. All rights reserved.
  
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

#include "utils/deadline.h"
#include <boost/date_time/posix_time/posix_time.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE deadline_test
#include <boost/test/unit_test.hpp>

static auto past = boost::posix_time::from_iso_string("19840101T000000");
static auto future = boost::posix_time::from_iso_string("30190101T000000");
static auto before_sometime = boost::posix_time::from_iso_string("20190313T154217,9999");
static auto sometime = boost::posix_time::from_iso_string("20190313T154218");
static auto after_sometime = boost::posix_time::from_iso_string("20190313T154218,0001");

BOOST_AUTO_TEST_CASE(simple_deadline) {
    auto deadline = navitia::Deadline();

    // an unitialised deadline never expire
    BOOST_CHECK_EQUAL(deadline.expired(past), false);
    BOOST_CHECK_EQUAL(deadline.expired(future), false);
    BOOST_REQUIRE_NO_THROW(deadline.check(past));
    BOOST_REQUIRE_NO_THROW(deadline.check(future));

    // we can set the deadline
    deadline.set(sometime);

    //in the past it's ok
    BOOST_CHECK_EQUAL(deadline.expired(past), false);

    // but not in the future
    BOOST_CHECK_EQUAL(deadline.expired(future), true);

    // even if it's only a little bit in the future
    BOOST_CHECK_EQUAL(deadline.expired(after_sometime), true);

    // a bit in the past is the past
    BOOST_CHECK_EQUAL(deadline.expired(before_sometime), false);

   // check() work like expire() but throw an exception
    BOOST_REQUIRE_THROW(deadline.check(after_sometime), navitia::DeadlineExpired);
    BOOST_REQUIRE_NO_THROW(deadline.check(before_sometime));
}


BOOST_AUTO_TEST_CASE(not_a_date_time_deadline) {
    auto deadline = navitia::Deadline(boost::posix_time::not_a_date_time);

    // since it isn't a datetime it should never expire
    BOOST_CHECK_EQUAL(deadline.expired(past), false);
    BOOST_CHECK_EQUAL(deadline.expired(future), false);

    // check() work like expire() but throw an exception
    BOOST_REQUIRE_NO_THROW(deadline.check(after_sometime));
    BOOST_REQUIRE_NO_THROW(deadline.check(before_sometime));
}

BOOST_AUTO_TEST_CASE(infinity_deadline) {
    auto deadline = navitia::Deadline(boost::posix_time::pos_infin);

    // the future if before the infinity
    BOOST_CHECK_EQUAL(deadline.expired(future), false);
    BOOST_REQUIRE_NO_THROW(deadline.check(future));

    deadline.set(boost::posix_time::neg_infin);
    //and the past if after the big bang
    BOOST_CHECK_EQUAL(deadline.expired(past), true);
    BOOST_REQUIRE_THROW(deadline.check(past), navitia::DeadlineExpired);
}
