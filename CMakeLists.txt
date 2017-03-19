cmake_minimum_required(VERSION 3.1.0)
project(FixedJSON)

#
# Build configuration for SimpleJSON itself...

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        include/SimpleJSON.h
        include/SimpleJSON.hpp
        src/SimpleJSON.cpp)

include_directories(dep/rapidjson/include dep/rapidjson/thirdparty/gtest/include include)

add_library(libFixedJSON STATIC ${SOURCE_FILES})

#
# The remaining is Test configuration
#

# Valgrind variables must be set before import the ctest module...
set( MEMORYCHECK_COMMAND_OPTIONS "--gen-suppressions=all --track-origins=yes --freelist-vol=2000000000 --error-exitcode=1 --track-fds=yes --num-callers=50 --fullpath-after= --trace-children=yes --leak-check=full" )
set( MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/valgrind_suppressions" )
find_program( MEMORYCHECK_COMMAND valgrind )

include(CTest)

add_subdirectory(dep/rapidjson/thirdparty/gtest EXCLUDE_FROM_ALL)

add_executable(jsonTests test/json.cpp)
target_link_libraries(jsonTests libFixedJSON gtest)

add_executable(jsonGenTests test/json_gen.cpp)
target_link_libraries(jsonGenTests libFixedJSON gtest)

enable_testing()
add_test(jsonTests jsonTests)
add_test(jsonGenTests jsonGenTests)

