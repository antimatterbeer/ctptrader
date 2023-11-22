cmake_minimum_required(VERSION 3.20)

project(ctptrader)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++20 -Wall -Werror -Wextra -fPIC -pedantic -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -DNDEBUG")

set(APP_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

set(THIRDPARTY ${CMAKE_SOURCE_DIR}/thirdparty)


include_directories(BEFORE SYSTEM ${CTP_INCLUDE_DIRS})
# required packages
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(GTest REQUIRED)
find_package(CURL REQUIRED)
find_package(fmt REQUIRED)
find_package(loguru REQUIRED)

set(CTP_INCLUDE_DIRS ${THIRDPARTY}/ctp/include)
set(CTP_LIBRARIES ${THIRDPARTY}/ctp/lib/libthostmduserapi_se.so ${THIRDPARTY}/ctp/lib/libthosttraderapi_se.so)

include_directories(BEFORE SYSTEM ${loguru_INCLUDE_DIRS})
include_directories(BEFORE SYSTEM ${fmt_INCLUDE_DIRS})
include_directories(BEFORE SYSTEM ${CTP_INCLUDE_DIRS})

enable_testing()

include_directories(ctptrader)
add_subdirectory(ctptrader)
add_subdirectory(demo)
