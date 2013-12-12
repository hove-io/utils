#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_test
#include <boost/test/unit_test.hpp>
#include "utils/flat_enum_map.h"
#include "utils/logger.h"
#include "utils/init.h"

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
    BOOST_CHECK_EQUAL(map[Mode::car], 0);
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

struct Value {
    Value() : val() {}
    Value(int i) : val(i) {}
    int val;
};

/**
  * test with an enum without a size last field
  *
  **/
BOOST_AUTO_TEST_CASE(flatEnumMap_no_size_test) {
    navitia::flat_enum_map<RawEnum, Value> map;

    map[RawEnum::second] = Value(42);

    BOOST_CHECK_EQUAL(map[RawEnum::second].val, 42);

    //default initialization
    BOOST_CHECK_EQUAL(map[RawEnum::first].val, 0);
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
