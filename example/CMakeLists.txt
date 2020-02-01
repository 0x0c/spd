cmake_minimum_required(VERSION 3.15.2)

project(sepd_example)
set(CMAKE_CXX_STANDARD 11)

add_executable(sepd_example main.cpp)

target_include_directories(sepd_example INTERFACE ../../include)