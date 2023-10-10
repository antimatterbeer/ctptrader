set(LIB_NAME serviceLib)

add_library(${LIB_NAME} STATIC
    ref.cpp
    dataService.cpp
)

target_link_libraries(${LIB_NAME} PRIVATE
    baseLib
)