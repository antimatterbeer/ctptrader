set(LIB_NAME logger)

add_library(${LIB_NAME} SHARED
    main_so.cpp
)

install(TARGETS ${LIB_NAME} DESTINATION ${APP_BIN_DIR})