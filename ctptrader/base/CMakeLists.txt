set(LIB_NAME baseLib)

add_library(${LIB_NAME} STATIC
    def.cpp
    date.cpp
    timestamp.cpp
    msg.cpp
    ref.cpp
)

add_executable(${LIB_NAME}_test 
    date_t.cpp
    timestamp_t.cpp    
)

target_link_libraries(${LIB_NAME}_test 
    ${LIB_NAME} 
    gtest
    gtest_main
)

add_test(NAME ${LIB_NAME}_test COMMAND ${LIB_NAME}_test)