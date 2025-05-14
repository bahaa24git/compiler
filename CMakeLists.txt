cmake_minimum_required(VERSION 3.29)
project(compilerCpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(compilerCpp old/main.cpp
        old/scanner.h
        old/scanner.cpp
        comp/project3_scanner.cpp
        comp/project3_parser.cpp
        comp/project3_parser_test.cpp
        comp/test_error_handling.cpp
        main.cpp
        scanner/scanner.cpp
        scanner/scanner.h
        parser/parser.cpp
        parser/parser.h
        common.h
)
