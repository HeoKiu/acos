cmake_minimum_required(VERSION 3.10)
project(inf24_1_fuse_mergefs C)

set(CMAKE_C_STANDARD 99)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse3)

link_libraries(${FUSE_LIBRARIES})         # -lfuse3 -lpthread
include_directories(${FUSE_INCLUDE_DIRS}) # -I/usr/include/fuse3

add_executable(inf24_1_fuse_mergefs main.c)

#target_include_directories(inf24_0_fuse_simplefs PUBLIC /home/boris/Downloads/fuse-3.10.3/include )
