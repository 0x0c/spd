cmake_minimum_required(VERSION 3.15.2)

project(spd_example)
set(CMAKE_CXX_STANDARD 11)

add_executable(spd_example main.cpp)

target_include_directories(spd_example INTERFACE ../../include)