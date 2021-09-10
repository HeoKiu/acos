cmake_minimum_required(VERSION 3.19)
project(inf25_0_openssl_dgst_sha512 C)

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
include_directories(inf25_0_openssl_dgst_sha512 ${CRYPTO_INCLUDE_DIR} )
add_executable(inf25_0_openssl_dgst_sha512 main.c)
include_directories(inf25_0_openssl_dgst_sha512 ${CRYPTO_INCLUDE_DIR} )
target_link_libraries(inf25_0_openssl_dgst_sha512 ${OPENSSL_CRYPTO_LIBRARY})