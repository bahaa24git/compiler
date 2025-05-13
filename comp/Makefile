# Makefile for compiling and running scanner + parser

CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: scanner parser

scanner: project3_scanner.cpp
	$(CXX) $(CXXFLAGS) project3_scanner.cpp -o scanner

parser: project3_parser.cpp
	$(CXX) $(CXXFLAGS) project3_parser.cpp -o parser

run: scanner parser
	./scanner test_input.txt > scanner_output.txt
	./parser

clean:
	rm -f scanner parser scanner_output.txt