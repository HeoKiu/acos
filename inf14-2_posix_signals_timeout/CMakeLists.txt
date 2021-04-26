cmake_minimum_required(VERSION 3.17)
project(inf14_2_posix_signals_timeout C)

set(CMAKE_C_STANDARD 99)

add_executable(inf14_2_posix_signals_timeout main.c generator.cpp)