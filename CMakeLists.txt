add_library(utils csv.cpp encoding_converter.cpp configuration.cpp timer.cpp functions.cpp paginate.h flat_enum_map.h init.h redis.cpp)

target_link_libraries(utils ${BOOST_LIBS} log4cplus)

add_subdirectory(tests)
