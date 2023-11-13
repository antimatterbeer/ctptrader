set(LIB_NAME utilLib)

add_library(${LIB_NAME} STATIC
    channel.cpp
    proxy.cpp
    csvReader.cpp
    ddbReader.cpp
)