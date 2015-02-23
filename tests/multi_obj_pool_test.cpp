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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE multi_obj_pool_test
#include <boost/test/unit_test.hpp>

#include "utils/multi_obj_pool.h"
#include <set>
#include <boost/range/algorithm/find.hpp>


BOOST_AUTO_TEST_CASE(one_obj) {
    std::function<bool(int, int)> f = [](int a, int b) { return a < b; };

    ParetoFront<int, std::function<bool(int, int)>> pool(f);

    BOOST_CHECK_EQUAL(pool.size(), 0);

    BOOST_CHECK(pool.add(1));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), 1);

    BOOST_CHECK(pool.add(2));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), 2);

    BOOST_CHECK(! pool.add(1));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), 2);
}

struct Bob {
    int bobette;
    int bobitto;

    bool operator==(const Bob& b) const { return b.bobette == bobette && b.bobitto == bobitto; }
};

std::ostream& operator<<(std::ostream& s, const Bob& b) {
    s << b.bobette << " | " << b.bobitto;
    return s;
}

template <typename Pool, typename Set>
bool is(const Pool& pool, Set list) {
    if (pool.size() != list.size()) {
        return false;
    }
    for (const auto& s: pool) {
        if (boost::find(list, s) == list.end())
            return false;
    }
    return true;
}

BOOST_AUTO_TEST_CASE(two_obj) {

    std::function<bool(Bob, Bob)> f = [](const Bob& a, const Bob& b) {
        //to be dominated, we need to have a worst bobette and a worst bobitto
        return a.bobette <= b.bobette && a.bobitto <= b.bobitto;
    };

    ParetoFront<Bob, std::function<bool(Bob, Bob)>> pool(f);
    BOOST_CHECK_EQUAL(pool.size(), 0);

    BOOST_CHECK(pool.add({1, 3}));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), Bob({1, 3}));

    //we add the same bob, so it's dominated by the other one, nothing added
    BOOST_CHECK(! pool.add({1, 3}));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), Bob({1, 3}));

    BOOST_CHECK(pool.add({3, 1}));
    BOOST_REQUIRE_EQUAL(pool.size(), 2);
    BOOST_CHECK(is(pool, std::vector<Bob>({Bob({1, 3}), Bob({3, 1})})));

    BOOST_CHECK(pool.add({1, 4})); //this dominates {1,3}, it will takes it's place
    BOOST_REQUIRE_EQUAL(pool.size(), 2);
    BOOST_CHECK(is(pool, std::vector<Bob>({Bob({1, 4}), Bob({3, 1})})));

    BOOST_CHECK(pool.add({0, 10})); //this is not dominated by the other 2, we add it
    BOOST_REQUIRE_EQUAL(pool.size(), 3);
    BOOST_CHECK(is(pool, std::vector<Bob>({Bob({1, 4}), Bob({3, 1}), Bob({0, 10})})));

    BOOST_CHECK(pool.add({10, 10})); //dominates everything
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK(is(pool, std::vector<Bob>({Bob({10, 10})})));
}

