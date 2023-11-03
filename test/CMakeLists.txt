enable_testing()

add_executable(channel_test channel_test.cpp)

target_link_libraries(channel_test PRIVATE
    baseLib
    utilLib
    gtest
    gtest_main
    Boost::system
)

add_test(NAME channel_test COMMAND channel_test)