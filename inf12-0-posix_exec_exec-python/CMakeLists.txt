cmake_minimum_required(VERSION 3.16)
project(inf12_0_posix_exec_exec_python C)

set(CMAKE_C_STANDARD 99)
include_directories(/home/boris/anaconda3/include/python3.8)

add_executable(inf12_0_posix_exec_exec_python main.c)