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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE pairs_generator_test

#include "utils/pairs_generator.h"
#include <boost/test/unit_test.hpp>

using namespace navitia::utils;

using std::pair;
using std::vector;

BOOST_AUTO_TEST_CASE(should_generate_pairs_from_a_container_std_style) {

    vector<int> v = {1, 2, 3};

    auto pairs = make_pairs_generator(v);
    auto it = pairs.begin();
    auto end = pairs.end();

    {
        BOOST_REQUIRE(it != end);
        auto pair = *it;
        BOOST_CHECK_EQUAL(*pair.first, 1);
        BOOST_CHECK_EQUAL(*pair.second, 2);
        ++it;
    }{
        BOOST_REQUIRE(it != end);
        auto pair = *it;
        BOOST_CHECK_EQUAL(*pair.first, 1);
        BOOST_CHECK_EQUAL(*pair.second, 3);
        ++it;
    }{
        BOOST_REQUIRE(it != end);
        auto pair = *it;
        BOOST_CHECK_EQUAL(*pair.first, 2);
        BOOST_CHECK_EQUAL(*pair.second, 3);
        ++it;
    }

    BOOST_CHECK(it == end);
}

BOOST_AUTO_TEST_CASE(shoud_generate_pairs_with_range_based_loop) {

    vector<int> s {1, 2, 3, 4};
    vector<pair<int, int>> pairs;

    auto pairs_gen = make_pairs_generator(s);

    for(auto pair : pairs_gen) {
        pairs.push_back({*pair.first, *pair.second});
    }

    vector<pair<int, int>> expected {
        {1, 2}, {1, 3}, {1, 4},
        {2, 3}, {2, 4}, {3, 4}
    };

    BOOST_REQUIRE_EQUAL(pairs.size(), expected.size());

    auto exp = expected.begin();
    for(auto & p : pairs) {
        BOOST_CHECK_EQUAL(p.first, exp->first);
        BOOST_CHECK_EQUAL(p.second, exp->second);
        ++exp;
    }
}

BOOST_AUTO_TEST_CASE(shoud_raise_exception_over_empty_container) {
    vector<int> s {};
    BOOST_CHECK_THROW( make_pairs_generator(s), std::length_error);
}

BOOST_AUTO_TEST_CASE(shoud_not_iterate_over_container_with_only_1_element) {
    vector<int> s { 1 };
    auto couples = make_pairs_generator(s);

    BOOST_CHECK( couples.begin() == couples.end() );
}
