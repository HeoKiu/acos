cmake_minimum_required(VERSION 3.16)
project(asm_x86_fpu_sse_fpu_sse_intro C ASM)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS -m32)
add_compile_options(
        -m32
)

add_executable(asm_x86_fpu_sse_fpu_sse_intro asm.S main.c)