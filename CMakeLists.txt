cmake_minimum_required(VERSION 3.29)
project(compiler C)

set(CMAKE_C_STANDARD 23)

add_executable(compiler main.c
        scanner.c
        scanner.h)
