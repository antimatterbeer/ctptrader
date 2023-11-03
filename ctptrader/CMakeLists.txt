add_subdirectory(base)
add_subdirectory(util)
add_subdirectory(core)
add_subdirectory(app)

add_executable(${PROJECT_NAME} ctptrader.cpp)

target_link_libraries(${PROJECT_NAME} 
    baseLib 
    utilLib 
    coreLib 
    appLib
    Boost::system
    ${THIRDPARTY}/ctp/lib/libthostmduserapi_se.so
    ${THIRDPARTY}/ctp/lib/libthosttraderapi_se.so
)