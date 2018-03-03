#
# Project Definition
#
cmake_minimum_required(VERSION 3.8.2)
project(FixedJSON)

#
# Find dependencies
#
find_package(Boost REQUIRED)
find_package(OSDevTools 0.1 REQUIRED)
find_package(UtilTime 0.1 REQUIRED)
find_package(DevToolsLog 0.1 REQUIRED)
find_package(RapidJSON 1.1.0 REQUIRED)

#
# Build configuration for SimpleJSON itself...
#
# Exported Library (libFixedJSON)
#
add_library(FixedJSON STATIC
        include/SimpleJSON.h
        include/SimpleJSON.hpp
        src/SimpleJSON.cpp)

target_link_libraries(FixedJSON PUBLIC
    DevToolsLog::Log
    OSDevTools::OSDevTools
    UtilTime::Time
    Boost::boost)
target_compile_features(FixedJSON PUBLIC cxx_std_14)

target_include_directories(FixedJSON PUBLIC
    $<BUILD_INTERFACE:${FixedJSON_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${RapidJSON_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:include>
)
#
# Generator
#
add_executable(jsonGen src/jsonGen.cpp)
target_link_libraries(jsonGen FixedJSON UtilTime::Time)

#
# Benchmarks
#
add_executable(example_dom benchmarks/example_dom.cpp)
target_link_libraries(example_dom FixedJSON UtilTime::Time)

add_executable(glossary_dom benchmarks/glossary_dom.cpp)
target_link_libraries(glossary_dom FixedJSON UtilTime::Time)

#
# Test Configuration
#
find_package(GTest REQUIRED)

add_executable(invalidJson test/invalidJson.cpp)
target_link_libraries(invalidJson FixedJSON GTest::GTest)

add_executable(json test/json.cpp)
target_link_libraries(json FixedJSON GTest::GTest)

add_executable(json_gen test/json_gen.cpp)
target_link_libraries(json_gen FixedJSON GTest::GTest)

#
# NOTE: Valgrind must be configured *before* testing is imported
#
set(MEMORYCHECK_COMMAND_OPTIONS "--gen-suppressions=all --track-origins=yes --freelist-vol=2000000000 --error-exitcode=1 --track-fds=yes --num-callers=50 --fullpath-after= --trace-children=yes --leak-check=full" )
find_program(MEMORYCHECK_COMMAND valgrind )
include (CTest)

enable_testing()
add_test(json json)
add_test(invalidJson invalidJson)
add_test(json_gen json_gen)

#
# Installation instructions
#
install(TARGETS FixedJSON EXPORT FixedJSONTargets
    ARCHIVE  DESTINATION lib
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include
)

install (EXPORT FixedJSONTargets
    FILE         FixedJSONTargets.cmake
    NAMESPACE    FixedJSON::
    DESTINATION  lib/cmake/FixedJSON
)

#
# Define our package version
#
include(CMakePackageConfigHelpers)
write_basic_package_version_file("FixedJSONConfigVersion.cmake"
    VERSION       0.1
    COMPATIBILITY SameMajorVersion
)
install (
    FILES       ${FixedJSON_BINARY_DIR}/FixedJSONConfigVersion.cmake
    DESTINATION lib/cmake/FixedJSON
)

#
# Finally export the Config which CMake will use
# when looking for matching libraries to link against.
#
install (
    FILES       FixedJSONConfig.cmake
    DESTINATION lib/cmake/FixedJSON
)

#
# Configure Coverage Build
#
SET(CMAKE_CXX_FLAGS_COVERAGE
    "${GCC_DEBUG_FLAGS} -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C++ compiler during coverage builds."
    FORCE )
SET(CMAKE_C_FLAGS_COVERAGE
    "${GCC_DEBUG_FLAGS} -fprofile-arcs -ftest-coverage"
    CACHE STRING "Flags used by the C compiler during coverage builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE)
