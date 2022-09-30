cmake_minimum_required(VERSION 2.8)
project(acle)

add_library(shapes shapes.cpp)

add_executable(Acle main.cpp)
target_link_libraries(Acle shapes)
