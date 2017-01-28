cmake_minimum_required(VERSION 3.5)
project(persistent_set)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(persistent_set.h persistent_set.cpp main.cpp)