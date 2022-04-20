/* Copyright © 2001-2018, Hove and/or its affiliates. All rights reserved.

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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE map_find_test

#include "utils/map_find.h"

#include <boost/test/unit_test.hpp>
#include <map>
#include <unordered_map>
#include <string>
#include <utility>

using namespace navitia::utils;
using std::string;

namespace {
    struct MapFindFixture
    {
        std::map<uint32_t, string> map;
        MapFindFixture() {
            map[1] = "one";
            map[2] = "two";
            map[3] = "three";
        }
    };
} // namespace

BOOST_FIXTURE_TEST_CASE(should_find_key_in_map, MapFindFixture) {

    string found_2;
    string not_found_2;

    make_map_find(map, 2)
        .if_found([&](const string& str){ found_2  = str; })
        .if_not_found([&](){ not_found_2 = "Not Found"; });

    BOOST_CHECK_EQUAL(found_2, "two");
    BOOST_CHECK_EQUAL(not_found_2, "");
}

BOOST_FIXTURE_TEST_CASE(should_not_find_key_not_in_map, MapFindFixture) {

    string found_42;
    string not_found_42;

    make_map_find(map, 42)
        .if_found([&](const string&){ found_42  = "forty two"; })
        .if_not_found([&](){ not_found_42 = "Not Found"; });

    BOOST_CHECK_EQUAL(found_42, "");
    BOOST_CHECK_EQUAL(not_found_42, "Not Found");
}

BOOST_FIXTURE_TEST_CASE(should_also_work_on_unordered_map, MapFindFixture) {

    std::unordered_map<uint32_t, string> umap;
    umap[1] = "one";
    umap[2] = "two";
    umap[3] = "three";

    string two;
    string forty_two;

    make_map_find(umap)
        .find(2)
            .if_found([&](const string& s){ two = s; })
            .if_not_found([&]()           { two = "Not found"; })
        .find(42)
            .if_found([&](const string& s){ forty_two = s; })
            .if_not_found([&]()           { forty_two = "Not found"; });

    BOOST_CHECK_EQUAL(two, "two");
    BOOST_CHECK_EQUAL(forty_two, "Not found");
}

BOOST_FIXTURE_TEST_CASE(should_not_copy_value_object_when_queried, MapFindFixture) {

    struct NonCopyable {
        const string str;
        NonCopyable(string str) : str(std::move(str)) {}

        /// We remove the copy semantic to make sure objects are not duplicated along the way
        NonCopyable(const NonCopyable &) = delete;
        NonCopyable& operator=(const NonCopyable &) = delete;

        /// we only keep the move construction semantic to emplace into the map
        NonCopyable(NonCopyable&& rhs) noexcept : str(std::move(rhs.str)) {}
        NonCopyable& operator=(NonCopyable && rhs) = delete;
    };

    std::map<int, NonCopyable> non_cop_map;
    non_cop_map.insert( std::make_pair(1, NonCopyable("one")) );
    non_cop_map.insert( std::make_pair(2, NonCopyable("two")) );
    non_cop_map.insert( std::make_pair(3, NonCopyable("three")) );

    string two;
    make_map_find(non_cop_map, 2)
        .if_found([&](const NonCopyable & s){ two = s.str; });

    BOOST_CHECK_EQUAL(two, "two");
}

BOOST_FIXTURE_TEST_CASE(should_find_key_in_const_map, MapFindFixture) {

    const auto const_map = map;
    string two;

    make_map_find(const_map, 2)
        .if_found([&](const string& s){ two = s; });

    BOOST_CHECK_EQUAL(two, "two");
}
