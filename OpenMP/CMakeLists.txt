cmake_minimum_required(VERSION 3.16)
project(OpenMP)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")


add_executable(OpenMP lab1.cpp lab2.cpp lab3.cpp lab4.cpp openmp.h lab5.cpp lab6.cpp)