cmake_minimum_required(VERSION 3.6)
project(logging)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES serverTCP.c clientTCP.c logDate2.h utilsock.h)
add_executable(logging ${SOURCE_FILES})