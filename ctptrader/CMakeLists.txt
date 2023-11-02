add_subdirectory(base)
add_subdirectory(util)
add_subdirectory(core)

add_executable(${PROJECT_NAME} ctptrader.cpp)

target_link_libraries(${PROJECT_NAME} 
    baseLib 
    utilLib 
    coreLib 
    Boost::system
)