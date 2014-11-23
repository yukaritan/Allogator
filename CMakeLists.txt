cmake_minimum_required(VERSION 2.8.4)
project(allocator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES allogator.cpp main.cpp)
add_executable(allocator ${SOURCE_FILES})