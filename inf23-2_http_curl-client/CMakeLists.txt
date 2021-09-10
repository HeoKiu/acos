cmake_minimum_required(VERSION 3.16)
project(inf23_2_http_curl_client C)

set(CMAKE_C_STANDARD 99)
set( CURL_LIBRARY ${CMAKE_SOURCE_DIR}/lib )
set( CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include )
find_package(CURL REQUIRED)

include_directories(${CURL_INCLUDE_DIR})
link_directories( ${CURL_LIBRARIES} )

add_executable(inf23_2_http_curl_client main.c)
target_link_libraries(inf23_2_http_curl_client curl)