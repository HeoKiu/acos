cmake_minimum_required(VERSION 3.16)
project(inf06_2_asm_x86_fpu_sse_vector_dot_product C ASM)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS -m32)
add_compile_options(
        -m32
)

add_executable(inf06_2_asm_x86_fpu_sse_vector_dot_product asm.s main.c)