cmake_minimum_required (VERSION 2.8.10)
project (h5enum C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wunreachable-code")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -ggdb3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2")

find_package (HDF5 REQUIRED COMPONENTS C)
include_directories (${HDF5_INCLUDE_DIRS})
include_directories (/usr/local/include)

add_executable(h5test h5enum.cc)
target_link_libraries(h5test ${HDF5_LIBRARIES})

find_package(NIX REQUIRED)
include_directories(${NIX_INCLUDE_DIRS})

add_executable(nixenum nixenum.cc)
target_link_libraries(nixenum ${NIX_LIBRARIES})
