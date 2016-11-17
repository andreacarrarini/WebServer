cmake_minimum_required(VERSION 3.6)
project(WebServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c ciao.h)
add_executable(WebServer ${SOURCE_FILES})