FIND_PACKAGE(Boost 1.44 COMPONENTS unit_test_framework thread regex serialization REQUIRED)

add_executable(utils_test utils_test.cpp)
target_link_libraries(utils_test utils ${Boost_LIBRARIES} log4cplus)

ADD_TEST(utils_test ${EXECUTABLE_OUTPUT_PATH}/utils_test --log_format=XML --log_sink=results_utils_test.xml --log_level=all --report_level=no)

add_executable(csvreader_test csvreader_test.cpp)
target_link_libraries(csvreader_test utils ${Boost_LIBRARIES} log4cplus)
ADD_TEST(csvreader_test ${EXECUTABLE_OUTPUT_PATH}/csvreader_test --log_format=XML --log_sink=results_utils_test.xml --log_level=all --report_level=no)