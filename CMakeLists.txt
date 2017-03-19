cmake_minimum_required(VERSION 3.5)
project(FixedJSON)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        include/SimpleJSON.h
        include/SimpleJSON.hpp
        src/SimpleJSON.cpp)

include_directories(dep/rapidjson/include dep/rapidjson/thirdparty/gtest/googletest/include include)

add_library(libFixedJSON STATIC ${SOURCE_FILES})

add_subdirectory(dep/rapidjson/thirdparty/gtest EXCLUDE_FROM_ALL)

add_executable(jsonTests test/json.cpp)
target_link_libraries(jsonTests libFixedJSON gtest)

add_executable(jsonGenTests test/json_gen.cpp)
target_link_libraries(jsonGenTests libFixedJSON gtest)
