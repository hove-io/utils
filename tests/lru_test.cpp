/* Copyright © 2001-2015, Canal TP and/or its affiliates. All rights reserved.
  
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

#include "utils/lru.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE lru_test
#include <boost/test/unit_test.hpp>

struct Fun {
    typedef int const& argument_type;
    using result_type = int;
    size_t& nb_call;
    Fun(size_t& nb): nb_call(nb) {}
    int operator()(const int& i) const { ++nb_call; return i * 2; }
};

BOOST_AUTO_TEST_CASE(simple_lru) {
    size_t nb_call = 0;

    // building a lru with no cache should throw
    BOOST_REQUIRE_THROW(navitia::make_lru(Fun(nb_call), 0), std::invalid_argument);

    // a Lru with 2 elements cached
    auto lru = navitia::make_lru(Fun(nb_call), 2);

    // cache = { 1 -> 2 }
    BOOST_CHECK_EQUAL(lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 1);
    BOOST_CHECK_EQUAL(lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 1);

    // cache = { 1 -> 2, 2 -> 4 }
    BOOST_CHECK_EQUAL(lru(2), 4);
    BOOST_CHECK_EQUAL(nb_call, 2);
    BOOST_CHECK_EQUAL(lru(2), 4);
    BOOST_CHECK_EQUAL(nb_call, 2);
    BOOST_CHECK_EQUAL(lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 2);

    // cache = { 1 -> 2, 3 -> 6 }
    // As "1" is the last used element, and the cache can only have 2
    // elements, the entry "2" is removed.
    BOOST_CHECK_EQUAL(lru(3), 6);
    BOOST_CHECK_EQUAL(nb_call, 3);
    BOOST_CHECK_EQUAL(lru(3), 6);
    BOOST_CHECK_EQUAL(nb_call, 3);
    BOOST_CHECK_EQUAL(lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 3);

    // cache = { 1 -> 2, 2 -> 4 }
    // As "2" was calculated earlier, but removed because of cache
    // size, we need to call the function to get the value.
    BOOST_CHECK_EQUAL(lru(2), 4);
    BOOST_CHECK_EQUAL(nb_call, 4);
    BOOST_CHECK_EQUAL(lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 4);
}

BOOST_AUTO_TEST_CASE(concurrent_lru) {
    size_t nb_call = 0;

    // a Lru with 2 elements cached
    auto lru = navitia::make_concurrent_lru(Fun(nb_call), 2);

    // cache = { 1 -> 2 }
    BOOST_CHECK_EQUAL(*lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 1);
    BOOST_CHECK_EQUAL(*lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 1);

    // cache = { 1 -> 2, 2 -> 4 }
    BOOST_CHECK_EQUAL(*lru(2), 4);
    BOOST_CHECK_EQUAL(nb_call, 2);
    BOOST_CHECK_EQUAL(*lru(2), 4);
    BOOST_CHECK_EQUAL(nb_call, 2);
    BOOST_CHECK_EQUAL(*lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 2);

    // cache = { 1 -> 2, 3 -> 6 }
    // As "1" is the last used element, and the cache can only have 2
    // elements, the entry "2" is removed.
    BOOST_CHECK_EQUAL(*lru(3), 6);
    BOOST_CHECK_EQUAL(nb_call, 3);
    BOOST_CHECK_EQUAL(*lru(3), 6);
    BOOST_CHECK_EQUAL(nb_call, 3);
    BOOST_CHECK_EQUAL(*lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 3);

    // cache = { 1 -> 2, 2 -> 4 }
    // As "2" was calculated earlier, but removed because of cache
    // size, we need to call the function to get the value.
    BOOST_CHECK_EQUAL(*lru(2), 4);
    BOOST_CHECK_EQUAL(nb_call, 4);
    BOOST_CHECK_EQUAL(*lru(1), 2);
    BOOST_CHECK_EQUAL(nb_call, 4);
}
