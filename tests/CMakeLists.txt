FIND_PACKAGE(Boost 1.44 COMPONENTS unit_test_framework thread regex serialization REQUIRED)


add_executable(utils_test utils_test.cpp)
target_link_libraries(utils_test utils ${Boost_LIBRARIES} log4cplus)
ADD_TEST(utils_test ${EXECUTABLE_OUTPUT_PATH}/utils_test --report_level=no)
