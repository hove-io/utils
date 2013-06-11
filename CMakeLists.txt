IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra -std=c++0x -Woverloaded-virtual -Wundef -rdynamic ")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(SRC csv.cpp encoding_converter.cpp configuration.cpp functions.cpp)
IF(NOT MSVC)
	SET(SRC ${SRC} timer.cpp)
ENDIF(NOT MSVC)

add_library(utils ${SRC})
