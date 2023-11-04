set(LIB_NAME coreLib)

add_library(${LIB_NAME} STATIC
    ref_center.cpp
    buf_center.cpp
    ctx.cpp
    stg.cpp
    app.cpp
)