#uri base64 encoding activated by giving a ENABLE_URI_ENCODING variable to cmake

IF(DEFINED ENABLE_URI_ENCODING)
    add_definitions(-DENABLE_URI_ENCODING)
ENDIF(DEFINED ENABLE_URI_ENCODING)

# ZMQ
FIND_PATH(ZMQ_INCLUDE_DIR zmq.hpp)
FIND_LIBRARY(ZMQ_LIB zmq)

# postgres
#
# we don't use find_package as it'll need pg_type.h, in
# postgresql-server-dev-9.x, but we only need libpq-dev
#FIND_PACKAGE(PostgreSQL REQUIRED)
FIND_PATH(PQ_INCLUDE_DIR postgresql/libpq-fe.h)
FIND_LIBRARY(PQ_LIB pq)

INCLUDE_DIRECTORIES(${ZMQ_INCLUDE_DIR} ${PQ_INCLUDE_DIR})

SET(UTILS_SRC
     csv.cpp
     encoding_converter.cpp
     configuration.cpp
     coord_parser.h
     coord_parser.cpp
     timer.cpp
     functions.cpp
     paginate.h
     flat_enum_map.h
     init.h
     backtrace.h
     multi_obj_pool.h
     base64_encode.cpp
     exception.h
     exception.cpp
     lotus.cpp
     threadbuf.h
     threadbuf.cpp
     get_hostname.cpp
     zmq.cpp
     deadline.cpp
)

add_library(utils ${UTILS_SRC})
target_link_libraries(utils ${Boost_REGEX_LIBRARY} ${Boost_THREAD_LIBRARY} ${PQ_LIB} log4cplus config ${ZMQ_LIB})

add_subdirectory(tests)
