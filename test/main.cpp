// filepath: /mnt/driveE/D/FCAI-H/3rd Junior/2nd term/Compilers/project/test/main.cpp
//
// Created by USER on 5/13/2025.
// Modified for functional programming on 5/15/2025.
//
#include <iostream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"

int main()
{
    // Use functional composition to build the compiler pipeline
    const string inputPath = "/mnt/driveE/D/FCAI-H/3rd Junior/2nd term/Compilers/project/compiler/test/input.txt";

    cout << "Scanner Output:\n";
    auto scanResult = Scanner::scanFile(inputPath);

    if (!scanResult.success)
    {
        cerr << "Scanning failed: " << scanResult.errorMessage << endl;
        return 1;
    }

    // Print tokens from the scanner
    Scanner::printTokens(scanResult.value.tokens);
    cout << endl
         << endl;

    // Pass tokens to parser
    cout << "Parser Output:\n";
    auto parseResult = Parser::parse(scanResult.value.tokens);

    // No need to explicitly print parser errors as they're printed during parsing

    return 0;
}