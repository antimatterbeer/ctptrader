set(LIB_NAME utilLib)

add_library(${LIB_NAME} STATIC
    channel.cpp
    proxy.cpp
    csvReader.cpp
    ddbReader.cpp
)

add_executable(${LIB_NAME}_test
    csvReader_t.cpp
    channel_t.cpp
)
target_link_libraries(${LIB_NAME}_test
    baseLib
    ${LIB_NAME}
    Boost::system
    gtest
    gtest_main
)
add_test(NAME ${LIB_NAME}_test COMMAND ${LIB_NAME}_test)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})