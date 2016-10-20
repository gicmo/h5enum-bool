cmake_minimum_required (VERSION 2.8.10)
project (h5enum C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wunreachable-code")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -ggdb3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2")

find_package (HDF5 REQUIRED COMPONENTS C)
include_directories (${HDF5_INCLUDE_DIRS})

add_executable(h5test h5enum.cc)
target_link_libraries(h5test ${HDF5_LIBRARIES})
