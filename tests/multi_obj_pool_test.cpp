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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE multi_obj_pool_test
#include <boost/test/unit_test.hpp>

#include "utils/multi_obj_pool.h"
#include <set>
#include <boost/range/algorithm/find.hpp>

template <typename Obj>
struct CountParetoFrontVisitor {
    size_t nb_is_dominated_by = 0;
    size_t nb_dominates = 0;
    size_t nb_inserted = 0;

    void is_dominated_by(const Obj& /*to_insert*/, const Obj& /*front_cur*/) { ++nb_is_dominated_by; }
    void dominates(const Obj& /*to_insert*/, const Obj& /*front_cur*/) { ++nb_dominates; }
    void inserted(const Obj& /*to_insert*/) { ++nb_inserted; }
};

BOOST_AUTO_TEST_CASE(one_obj) {
    std::function<bool(int, int)> f = [](int a, int b) { return a >= b; };

    ParetoFront<int, std::function<bool(int, int)>, CountParetoFrontVisitor<int>> pool(f);
    const auto& visitor = pool.getV();

    BOOST_CHECK_EQUAL(pool.size(), 0);

    BOOST_CHECK(pool.add(1));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), 1);
    BOOST_CHECK_EQUAL(visitor.nb_inserted, 1);
    BOOST_CHECK_EQUAL(visitor.nb_dominates, 0);
    BOOST_CHECK_EQUAL(visitor.nb_is_dominated_by, 0);

    BOOST_CHECK(pool.add(2));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), 2);
    BOOST_CHECK_EQUAL(visitor.nb_inserted, 2);
    BOOST_CHECK_EQUAL(visitor.nb_dominates, 1);
    BOOST_CHECK_EQUAL(visitor.nb_is_dominated_by, 0);

    BOOST_CHECK(! pool.add(1));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), 2);
    BOOST_CHECK_EQUAL(visitor.nb_inserted, 2);
    BOOST_CHECK_EQUAL(visitor.nb_dominates, 1);
    BOOST_CHECK_EQUAL(visitor.nb_is_dominated_by, 1);

    BOOST_CHECK(! pool.add(2));
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK_EQUAL(*pool.begin(), 2);
    BOOST_CHECK_EQUAL(visitor.nb_inserted, 2);
    BOOST_CHECK_EQUAL(visitor.nb_dominates, 1);
    BOOST_CHECK_EQUAL(visitor.nb_is_dominated_by, 2);

    BOOST_CHECK(pool.is_dominated(1));
    BOOST_CHECK(pool.is_dominated(2));
    BOOST_CHECK(!pool.is_dominated(3));
}

struct Bob {
    int bobette;
    int bobitto;

    bool operator==(const Bob& b) const { return b.bobette == bobette && b.bobitto == bobitto; }
    bool operator<(const Bob& b) const {
        if (b.bobette != bobette) {
            return b.bobette < bobette;
        }
        return b.bobitto < bobitto;
    }
};

static std::ostream& operator<<(std::ostream& s, const Bob& b) {
    s << b.bobette << " | " << b.bobitto;
    return s;
}

template <typename Pool>
bool is(const Pool& pool, std::set<typename Pool::value_type> list) {
    if (pool.size() != list.size()) {
        return false;
    }
    std::set<typename Pool::value_type> p;
    for (const auto& s: pool) {
        p.insert(s);
    }
    return p == list;
}

BOOST_AUTO_TEST_CASE(two_obj) {

    std::function<bool(Bob, Bob)> f = [](const Bob& a, const Bob& b) {
        //to be dominated, we need to have a worst bobette and a worst bobitto
        return a.bobette >= b.bobette && a.bobitto >= b.bobitto;
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
    BOOST_CHECK(is(pool, {Bob({1, 3}), Bob({3, 1})}));

    BOOST_CHECK(pool.add({1, 4})); //this dominates {1,3}, it will takes it's place
    BOOST_REQUIRE_EQUAL(pool.size(), 2);
    BOOST_CHECK(is(pool, {Bob({1, 4}), Bob({3, 1})}));

    BOOST_CHECK(pool.add({0, 10})); //this is not dominated by the other 2, we add it
    BOOST_REQUIRE_EQUAL(pool.size(), 3);
    BOOST_CHECK(is(pool, {Bob({1, 4}), Bob({3, 1}), Bob({0, 10})}));

    BOOST_CHECK(! pool.add({1, 3})); //this is weakly dominated, should not be added
    BOOST_REQUIRE_EQUAL(pool.size(), 3);
    BOOST_CHECK(is(pool, {Bob({1, 4}), Bob({3, 1}), Bob({0, 10})}));

    BOOST_CHECK(pool.add({10, 10})); //dominates everything
    BOOST_REQUIRE_EQUAL(pool.size(), 1);
    BOOST_CHECK(is(pool, {Bob({10, 10})}));
}

