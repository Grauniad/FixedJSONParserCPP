cmake_minimum_required(VERSION 3.6)
project(FixedJSON)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        include/SimpleJSON.h
        include/SimpleJSON.hpp
        src/SimpleJSON.cpp)

include_directories(dep/rapidjson/include include)

add_library(libFixedJSON STATIC ${SOURCE_FILES})