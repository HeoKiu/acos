cmake_minimum_required(VERSION 3.16)
project(inf05_2__asm_x86_mergesort C ASM)

set(CMAKE_C_STANDARD 99)

add_executable(inf05_2__asm_x86_mergesort asm.S main.c)