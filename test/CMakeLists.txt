
add_executable(util_test util_test.cpp)

target_link_libraries(util_test PRIVATE
    baseLib
    utilLib
    gtest
    gtest_main
    Boost::system
)

add_test(NAME util_test COMMAND util_test)

add_executable(core_test core_test.cpp)

target_link_libraries(core_test PRIVATE
    baseLib
    utilLib
    coreLib
    gtest
    gtest_main
    Boost::system
)

add_test(NAME core_test COMMAND core_test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fixtures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
