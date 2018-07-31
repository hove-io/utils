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

#include "utils/idx_map.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE idx_map_test
#include <boost/test/unit_test.hpp>

// a non copyable, non default constructible object with an idx field
struct Bob {
    Bob() = delete;
    Bob(const Bob&) = delete;
    Bob& operator=(const Bob&) = delete;
    Bob(Bob&&) = default;
    Bob(navitia::idx_t i): idx(i) {}
    navitia::idx_t idx;
};

BOOST_AUTO_TEST_CASE(idx_constructor) {
    const Bob bob(int(0));
    navitia::Idx<Bob> idx = navitia::Idx<Bob>(bob);
    BOOST_CHECK_EQUAL(idx.val, 0);
}

BOOST_AUTO_TEST_CASE(idx_map_iterations_read_write) {
    //we need a container to back our idx map
    std::vector<Bob> bob_container;
    for (size_t i = 0; i < 42; ++i) {
        bob_container.emplace_back(Bob(i));
    }

    navitia::IdxMap<Bob, int> map;
    map.assign(bob_container, 0);
    navitia::idx_t idx = 0;

    // mut iteration
    for (auto elt: map) {
        BOOST_CHECK_EQUAL(elt.first, navitia::Idx<Bob>(Bob(idx)));
        BOOST_CHECK_EQUAL(elt.second, 0);
        elt.second = int(idx + 10);
        BOOST_CHECK_EQUAL(elt.second, int(idx + 10));
        ++idx;
    }

    // mut access
    for (idx = 0; idx < 42; ++idx) {
        const auto key = navitia::Idx<Bob>(Bob(idx));
        BOOST_CHECK_EQUAL(map[key], int(idx + 10));
        map[key] += 10;
        BOOST_CHECK_EQUAL(map[key], int(idx + 20));
    }

    // mut values iteration
    idx = 0;
    for (auto& v: map.values()) {
        BOOST_CHECK_EQUAL(v, int(idx + 20));
        v -= 10;
        BOOST_CHECK_EQUAL(v, int(idx + 10));
        ++idx;
    }

    const auto& const_map = map;

    // const iteration
    idx = 0;
    for (const auto elt: const_map) {
        BOOST_CHECK_EQUAL(elt.first, navitia::Idx<Bob>(Bob(idx)));
        BOOST_CHECK_EQUAL(elt.second, int(idx + 10));
        ++idx;
    }

    // const access
    for (idx = 0; idx < 42; ++idx) {
        const auto key = navitia::Idx<Bob>(Bob(idx));
        BOOST_CHECK_EQUAL(map[key], int(idx + 10));
    }

    // const values iteration
    idx = 0;
    for (auto& v: map.values()) {
        BOOST_CHECK_EQUAL(v, int(idx + 10));
        ++idx;
    }
}
