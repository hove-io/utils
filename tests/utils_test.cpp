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
#define BOOST_TEST_MODULE utils_test
#include <boost/test/unit_test.hpp>
#include <set>
#include <vector>
#include "utils/flat_enum_map.h"
#include "utils/logger.h"
#include "utils/init.h"
#include "utils/base64_encode.h"
#include "utils/functions.h"

struct logger_initialized {
    logger_initialized()   { init_logger(); }
};
BOOST_GLOBAL_FIXTURE( logger_initialized )

enum class Mode {
    bike = 0,
    walk,
    car,
    size
};

/**
  * simple test for the enum map
  *
  **/
BOOST_AUTO_TEST_CASE(flatEnumMap_simple_test) {
    navitia::flat_enum_map<Mode, int> map;

    map[Mode::bike] = 2;

    BOOST_CHECK_EQUAL(map[Mode::bike], 2);

    //default initialization
    //due to a gcc bug http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57086 the container cannont be
    // default initialized any longer cf comment in flat_enum_map
//    BOOST_CHECK_EQUAL(map[Mode::car], 0);
}

enum class RawEnum {
    first = 0,
    second,
    last
};

namespace navitia {
template <>
struct enum_size_trait<RawEnum> {
    static constexpr typename get_enum_type<RawEnum>::type size() {
        return 3;
    }
};
}

std::ostream& operator<<(std::ostream& o, RawEnum e) {
    return o << static_cast<int>(e);
}
std::ostream& operator<<(std::ostream& o, Mode e) {
    return o << static_cast<int>(e);
}

struct Value {
    Value() : val() {}
    Value(int i) : val(i) {}
    int val;
};

BOOST_AUTO_TEST_CASE(enum_iterator) {
    auto it = navitia::enum_iterator<RawEnum>(RawEnum::first);
    BOOST_CHECK(it != navitia::enum_iterator<RawEnum>());
    BOOST_CHECK_EQUAL(*it, RawEnum::first);
    BOOST_CHECK(it != navitia::enum_iterator<RawEnum>());
    BOOST_CHECK_EQUAL(*++it, RawEnum::second);
    BOOST_CHECK(it != navitia::enum_iterator<RawEnum>());
    BOOST_CHECK_EQUAL(*++it, RawEnum::last);
    BOOST_CHECK(++it == navitia::enum_iterator<RawEnum>()); //invalid iterator
}

BOOST_AUTO_TEST_CASE(enum_range) {
    std::vector<RawEnum> res;

    for (auto e: navitia::enum_range<RawEnum>()) {
        res.push_back(e);
    }
    std::vector<RawEnum> wanted_res {RawEnum::first, RawEnum::second, RawEnum::last};

    BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(res), std::end(res), std::begin(wanted_res), std::end(wanted_res));
}

BOOST_AUTO_TEST_CASE(enum_reverse_range) {
    std::vector<Mode> res;

    for (auto e: navitia::reverse_enum_range<Mode>()) {
        res.push_back(e);
    }
    std::vector<Mode> wanted_res {Mode::car, Mode::walk, Mode::bike};

    BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(res), std::end(res), std::begin(wanted_res), std::end(wanted_res));
}

BOOST_AUTO_TEST_CASE(enum_reverse_range_from) {
    std::vector<Mode> res;

    for (auto e: navitia::reverse_enum_range_from<Mode>(Mode::walk)) {
        res.push_back(e);
    }
    std::vector<Mode> wanted_res {Mode::walk, Mode::bike};

    BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(res), std::end(res), std::begin(wanted_res), std::end(wanted_res));
}
/**
  * test with an enum without a size last field
  *
  **/
BOOST_AUTO_TEST_CASE(flatEnumMap_no_size_test) {
    navitia::flat_enum_map<RawEnum, Value> map;

    map[RawEnum::second] = Value(42);

    BOOST_CHECK_EQUAL(map[RawEnum::second].val, 42);

    //default initialization
    //due to a gcc bug http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57086 the container cannont be
    // default initialized any longer
//    BOOST_CHECK_EQUAL(map[RawEnum::first].val, 0);
}

/**
  * test with an initilizer construction
  * The test is more that it should compile :)
  **/
BOOST_AUTO_TEST_CASE(flatEnumMap_initializer) {
    const navitia::flat_enum_map<RawEnum, int> map = {{{1, 3, 4}}}; //yes 3 curly braces :) one for the flat_enum_map and 2 for the underlying array

    BOOST_CHECK_EQUAL(map[RawEnum::first], 1);
    BOOST_CHECK_EQUAL(map[RawEnum::second], 3);
    BOOST_CHECK_EQUAL(map[RawEnum::last], 4);
}

/**
  * basic test for iterator
  *
  **/
BOOST_AUTO_TEST_CASE(flatEnumMap_iterator_test) {
    navitia::flat_enum_map<RawEnum, int> map;

    map[RawEnum::first] = 4;
    map[RawEnum::second] = 42;
    map[RawEnum::last] = 420;

    std::vector<int> expected {4, 42, 420};
    std::vector<RawEnum> expectedEnum {RawEnum::first, RawEnum::second, RawEnum::last};
    std::vector<int> val;
    std::vector<RawEnum> enumVal;

    for (const auto& pair : map) {
        val.push_back(pair.second);
        enumVal.push_back(pair.first);
    }

    BOOST_CHECK(val == expected);
    BOOST_CHECK(enumVal == expectedEnum);
}

BOOST_AUTO_TEST_CASE(encode_uri_test) {
    BOOST_REQUIRE_EQUAL(navitia::base64_encode("line:RTP:1000387"), "bGluZTpSVFA6MTAwMDM4Nw");
}

BOOST_AUTO_TEST_CASE(natural_sort_test) {
    std::vector<std::string> list {
        "toto",
        "tutu",
        "tutu10",
        "tutu2",
        "15",
        "25",
        "5"
    };

    std::sort(list.begin(), list.end(), navitia::pseudo_natural_sort());

    int i = 0;
    BOOST_CHECK_EQUAL(list[i++], "5");
    BOOST_CHECK_EQUAL(list[i++], "15");
    BOOST_CHECK_EQUAL(list[i++], "25");
    BOOST_CHECK_EQUAL(list[i++], "toto");
    BOOST_CHECK_EQUAL(list[i++], "tutu");
    //we wont be able to sort  tutu10 and tutu2
    //(in a pure natural order tutu10 is after tutu2, but it's too complicated for our need)
    BOOST_CHECK_EQUAL(list[i++], "tutu10");
    BOOST_CHECK_EQUAL(list[i++], "tutu2");
}

struct MockedContainerWithFind {
    struct iterator{};
    iterator end() const{return iterator();}
    bool mutable find_is_called{false};
    iterator find(int) const {
        find_is_called = true;
        return iterator();
    }
};

inline bool operator!=(const MockedContainerWithFind::iterator&,
                       const MockedContainerWithFind::iterator&) {
    return false;
}

/*
 * Test the behavior of contains. Especially, when 'find' is implemented
 * */
BOOST_AUTO_TEST_CASE(contains_test) {
    std::set<int> s{0, 1, 2};
    BOOST_CHECK_EQUAL(navitia::contains(s, 0), true);
    BOOST_CHECK_EQUAL(navitia::contains(s, 3), false);
    std::vector<int> v{0, 1, 2};
    BOOST_CHECK_EQUAL(navitia::contains(v, 0), true);
    BOOST_CHECK_EQUAL(navitia::contains(v, 3), false);

    MockedContainerWithFind mocked_container;
    navitia::contains(mocked_container, 4);
    BOOST_CHECK_EQUAL(mocked_container.find_is_called, true);
}
