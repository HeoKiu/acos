cmake_minimum_required(VERSION 3.17)
project(memaccess C)

set(CMAKE_C_STANDARD 99)

add_executable(memaccess main.s asm_funcs.h main.c)