add_library(crypto
    CryptoUtils.cpp CryptoUtils.h
)
target_include_directories(crypto PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(crypto PUBLIC OpenSSL::Crypto)
