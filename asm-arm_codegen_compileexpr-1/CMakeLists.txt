cmake_minimum_required(VERSION 3.16)
project(asm_arm_codegen_compileexpr_1)

set(CMAKE_CXX_STANDARD 17)

add_executable(asm_arm_codegen_compileexpr_1 jit_compile_arm.cpp test.c tests.cpp tests.h)