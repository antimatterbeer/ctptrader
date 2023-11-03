
add_executable(channel_test channel_test.cpp)

target_link_libraries(channel_test PRIVATE
    baseLib
    utilLib
    gtest
    gtest_main
    Boost::system
)

add_test(NAME channel_test COMMAND channel_test)

add_executable(data_ref_test data_ref_test.cpp)

target_link_libraries(data_ref_test PRIVATE
    baseLib
    utilLib
    coreLib
    gtest
    gtest_main
    Boost::system
)

add_test(NAME data_ref_test COMMAND data_ref_test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fixtures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
