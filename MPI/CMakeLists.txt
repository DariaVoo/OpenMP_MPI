cmake_minimum_required(VERSION 3.16)
project(MPI C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_COMPILER "/usr/bin/mpicc")
set(CMAKE_CXX_COMPILER «/usr/bin/mpic++»)
# Include MPI includes
include_directories(/usr/include/mpi/)


add_executable(MPI
        labs123.c
        lab4.c
        mpi_labs.h
        lab5.c
        lab6.c)
