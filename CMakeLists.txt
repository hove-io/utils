IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra -std=c++0x -Woverloaded-virtual -Wundef -rdynamic ")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

add_library(utils csv.cpp encoding_converter.cpp configuration.cpp timer.cpp functions.cpp paginate.h)
