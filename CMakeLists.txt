SET(UTILS_SRC
     csv.cpp
     encoding_converter.cpp
     configuration.cpp
     timer.cpp
     functions.cpp
     paginate.h
     flat_enum_map.h
     init.h
     backtrace.h
     redis.cpp
     base64_encode.cpp
)

add_library(utils ${UTILS_SRC})

target_link_libraries(utils ${BOOST_LIBS} log4cplus)

add_subdirectory(tests)
