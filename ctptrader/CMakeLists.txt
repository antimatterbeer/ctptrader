add_subdirectory(base)
add_subdirectory(util)
add_subdirectory(service)
add_subdirectory(app)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    baseLib
    utilLib
    serviceLib
    appLib
    ${THIRDPARTY}/ctp/lib/libthostmduserapi_se.so
    ${THIRDPARTY}/ctp/lib/libthosttraderapi_se.so
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${APP_BIN_DIR})