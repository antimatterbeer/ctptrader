add_executable(serviceTest serviceTest.cpp)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fixtures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(serviceTest PRIVATE
    utilLib
    serviceLib
    gtest
    gtest_main
)

add_test(NAME serviceTest COMMAND serviceTest)
