cmake_minimum_required(VERSION 3.6)
project(WebServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES server.c cache_functions.c structs.h thread_cond_functions.c error_functions.c functions.h)
add_executable(WebServer ${SOURCE_FILES})