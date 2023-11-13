# csv-parser
include_directories(BEFORE SYSTEM ${THIRDPARTY}/csv-parser/include)

# spdlog
include_directories(BEFORE SYSTEM ${THIRDPARTY}/spdlog/include)

# fmt
include_directories(BEFORE SYSTEM ${THIRDPARTY}/fmt/include)

# rapidjson
include_directories(BEFORE SYSTEM ${THIRDPARTY}/rapidjson/include)

# cmdline
include_directories(BEFORE SYSTEM ${THIRDPARTY}/cmdline/include)

# toml++
include_directories(BEFORE SYSTEM ${THIRDPARTY}/tomlplusplus/include)

# ctp
include_directories(BEFORE SYSTEM ${THIRDPARTY}/ctp/include)
set(CTP_LIBRARIES ${THIRDPARTY}/ctp/lib/libthostmduserapi_se.so ${THIRDPARTY}/ctp/lib/libthosttraderapi_se.so)

# dolphindb
include_directories(BEFORE SYSTEM ${THIRDPARTY}/dolphindb/include)
set(DOLPHINDB_LIBRARIES 
    ${THIRDPARTY}/dolphindb/lib/libDolphinDBAPI.so
    ${THIRDPARTY}/dolphindb/lib/libcrypto.so
    ${THIRDPARTY}/dolphindb/lib/libssl.so
    ${THIRDPARTY}/dolphindb/lib/libuuid.a
)