cmake_minimum_required(VERSION 3.16)
project(inf06_3_generic_sse_matrix_mul C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS -msse4.1)
add_compile_options(
        -msse4.1
)

add_executable(inf06_3_generic_sse_matrix_mul main.c)