cmake_minimum_required(VERSION 3.17)
project(inf25-1_openssl_decrypt-aes-256-cbc C)

set(CMAKE_C_STANDARD 99)

find_package(OpenSSL COMPONENTS Crypto REQUIRED)
pkg_search_module(CRYPTO libcrypto REQUIRED)
include_directories(${CRYPTO_INCLUDE_DIRS})
find_path(CRYPTO_INCLUDE_DIR openssl/md5.h)

find_library(CRYPTO_LIBRARY crypto)
pkg_search_module(OPENSSL openssl REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIRS})


pkg_search_module(CRYPTO libcrypto REQUIRED)
include_directories(${CRYPTO_INCLUDE_DIRS})
include_directories(inf25-1_openssl_decrypt-aes-256-cbc ${CRYPTO_INCLUDE_DIR} )
add_executable(inf25-1_openssl_decrypt-aes-256-cbc main.c)
include_directories(inf25-1_openssl_decrypt-aes-256-cbc ${CRYPTO_INCLUDE_DIR} )
target_link_libraries(inf25-1_openssl_decrypt-aes-256-cbc ${OPENSSL_CRYPTO_LIBRARY})