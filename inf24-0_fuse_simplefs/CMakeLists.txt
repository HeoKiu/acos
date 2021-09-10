cmake_minimum_required(VERSION 3.16)
project(c)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall")

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse3)
link_libraries(${FUSE_LIBRARIES})
include_directories(${FUSE_INCLUDE_DIRS})

include_directories(SYSTEM /usr/include)
add_executable(c main.c)
target_link_libraries(c rt pthread dl)