cmake_minimum_required(VERSION 3.29)
project(compilerCpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(compilerCpp main.cpp
        scanner.h
        scanner.cpp)
