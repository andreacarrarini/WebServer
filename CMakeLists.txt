cmake_minimum_required(VERSION 3.6)
project(WebServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES server.c cache_functions.c thread_functions.c error_functions.c functions.h file_functions.c image_functions.c structs.h functions.h)
add_executable(WebServer ${SOURCE_FILES})

find_package(ImageMagick COMPONENTS ImageWand)

include_directories(/usr/local/include/ImageMagick-7)