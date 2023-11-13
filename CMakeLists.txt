cmake_minimum_required(VERSION 3.20)

project(ctptrader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++20 -Wall -Werror -Wextra -fPIC -pedantic -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -DNDEBUG")

set(APP_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# required packages
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(GTest REQUIRED)
find_package(CURL REQUIRED)

# third party
set(THIRDPARTY ${CMAKE_SOURCE_DIR}/thirdparty)
include(${THIRDPARTY}/CMakeLists.txt)

enable_testing()

include_directories(ctptrader)
add_subdirectory(ctptrader)
add_subdirectory(demo)
add_subdirectory(test)
