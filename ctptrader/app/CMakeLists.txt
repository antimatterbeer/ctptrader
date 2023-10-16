set(LIB_NAME appLib)

add_library(${LIB_NAME} STATIC
    stgManager.cpp
    ctpMG.cpp
    ctpTG.cpp
)

target_link_libraries(${LIB_NAME}
    baseLib
    utilLib
    serviceLib
    ${THIRDPARTY}/ctp/lib/libthostmduserapi_se.so
    ${THIRDPARTY}/ctp/lib/libthosttraderapi_se.so
    ${THIRDPARTY}/fmt/lib/libfmt.a
)
