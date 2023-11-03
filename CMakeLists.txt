cmake_minimum_required(VERSION 3.20)

project(ctptrader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++20 -fPIC -Wall -Werror -Wno-unused-function -Wno-sign-compare -Wno-write-strings -pedantic -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -DNDEBUG")

set(APP_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# required packages
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(GTest REQUIRED)
find_package(CURL REQUIRED)

# third party
set(THIRDPARTY ${CMAKE_SOURCE_DIR}/thirdparty)
include_directories(${THIRDPARTY}/csv-parser/include)
include_directories(${THIRDPARTY}/spdlog/include)
include_directories(${THIRDPARTY}/tomlplusplus/include)
include_directories(${THIRDPARTY}/fmt/include)
include_directories(${THIRDPARTY}/rapidjson/include)
include_directories(${THIRDPARTY}/cmdline/include)
include_directories(${THIRDPARTY}/tomlplusplus/include)
include_directories(${THIRDPARTY}/ctp/include)

enable_testing()

include_directories(ctptrader)
add_subdirectory(ctptrader)
add_subdirectory(demo)
add_subdirectory(test)
