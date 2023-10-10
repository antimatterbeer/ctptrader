set(LIB_NAME utilLib)

add_library(${LIB_NAME} STATIC
    channel.cpp
    csvReader.cpp
    httpClient.cpp
)

target_link_libraries(${LIB_NAME} PRIVATE
    boost_system
    boost_thread
    ssl
    crypto
    ${CURL_LIBRARIES}
)
