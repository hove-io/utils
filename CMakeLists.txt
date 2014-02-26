#uri base64 encoding activated by giving a ENABLE_URI_ENCODING variable to cmake
IF(DEFINED ENABLE_URI_ENCODING)
    add_definitions(-DENABLE_URI_ENCODING)
ENDIF(DEFINED ENABLE_URI_ENCODING)

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
     base64_encode.cpp
)

add_library(utils ${UTILS_SRC})

target_link_libraries(utils ${BOOST_LIBS} log4cplus)

add_subdirectory(tests)
