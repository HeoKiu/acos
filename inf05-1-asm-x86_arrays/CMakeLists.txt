cmake_minimum_required(VERSION 3.16)
project(inf05_1_asm_x86_arrays C ASM)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS -m32)
add_compile_options(
        -m32
)

add_executable(inf05_1_asm_x86_arrays asm.S main.c)