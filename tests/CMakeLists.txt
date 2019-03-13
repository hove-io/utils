FIND_PACKAGE(Boost 1.55 COMPONENTS unit_test_framework thread regex serialization date_time REQUIRED)

add_executable(utils_test utils_test.cpp)
target_link_libraries(utils_test utils ${Boost_LIBRARIES} log4cplus)

add_boost_test(utils_test)

add_executable(csvreader_test csvreader_test.cpp)
target_link_libraries(csvreader_test utils ${Boost_LIBRARIES} log4cplus)
add_boost_test(csvreader_test)

add_executable(lru_test lru_test.cpp)
target_link_libraries(lru_test ${Boost_LIBRARIES} log4cplus)
add_boost_test(lru_test)

add_executable(idx_map_test idx_map_test.cpp)
target_link_libraries(idx_map_test ${Boost_LIBRARIES} log4cplus)
add_boost_test(idx_map_test)

add_executable(multi_obj_pool_test multi_obj_pool_test.cpp)
target_link_libraries(multi_obj_pool_test utils ${Boost_LIBRARIES} log4cplus)
add_boost_test(multi_obj_pool_test)

add_executable(obj_factory_test obj_factory_test.cpp)
target_link_libraries(obj_factory_test utils ${Boost_LIBRARIES} log4cplus)
add_boost_test(obj_factory_test)

add_executable(map_find_test map_find_test.cpp)
target_link_libraries(map_find_test utils ${Boost_LIBRARIES} log4cplus)
add_boost_test(map_find_test)

add_executable(deadline_test deadline_test.cpp)
target_link_libraries(deadline_test utils ${Boost_LIBRARIES} log4cplus)
add_boost_test(deadline_test)
