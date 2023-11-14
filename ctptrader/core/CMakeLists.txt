set(LIB_NAME coreLib)

add_library(${LIB_NAME} STATIC
    ctx.cpp
    stg.cpp
    app.cpp
    factor.cpp
    reader.cpp
)

add_executable(${LIB_NAME}_test ctx_t.cpp)

target_link_libraries(${LIB_NAME}_test 
    baseLib
    utilLib
    ${LIB_NAME} 
    gtest
    gtest_main
)

add_test(NAME ${LIB_NAME}_test COMMAND ${LIB_NAME}_test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test DESTINATION ${CMAKE_CURRENT_BINARY_DIR})