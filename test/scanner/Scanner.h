//
// Created by USER on 5/13/2025.
// Modified for functional programming on 5/15/2025.
//

#ifndef SCANNER_H
#define SCANNER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <vector>
#include <stack>
#include <filesystem>
#include <set>
#include "../common.h"

using namespace std;

namespace Scanner
{
    // Types for scanner state (immutable)
    struct ScannerContext
    {
        unordered_map<string, string> keywords;
        bool inMultiLineComment;
        set<string> processedFiles;

        static ScannerContext create()
        {
            unordered_map<string, string> kw = {
                {"IfTrue", "Condition"},
                {"Otherwise", "Condition"},
                {"Imw", "Integer"},
                {"SIMw", "SInteger"},
                {"Chj", "Character"},
                {"Series", "String"},
                {"IMwf", "Float"},
                {"SIMwf", "SFloat"},
                {"NOReturn", "Void"},
                {"RepeatWhen", "Loop"},
                {"Reiterate", "Loop"},
                {"Turnback", "Return"},
                {"OutLoop", "Break"},
                {"Loli", "Struct"},
                {"Include", "Inclusion"},
            };
            return {kw, false, {}};
        }
    };

    struct ScanResult
    {
        TokenCollection tokens;
        int errorCount;
        ScannerContext context;
    };

    // Pure functions
    bool isValidIdentifier(const string &token);
    bool isNumber(const string &token);

    // Token creation function
    Token createToken(int lineNumber, const string &text, const string &type);

    // Process a token and determine its type
    TokenCollection processToken(const ScannerContext &context, int lineNumber, const string &token, TokenCollection tokens);

    // Tokenize a line character by character
    Result<ScanResult> tokenizeLine(const ScannerContext &context, const string &line, int lineNumber, TokenCollection tokens);

    // Handle include directives
    Result<ScanResult> handleInclude(const ScannerContext &context, const string &line, const string &currentFile, int lineNumber, TokenCollection tokens);

    // Process a single line of source code
    Result<ScanResult> scanLine(const ScannerContext &context, const string &line, const string &filename, int lineNumber, TokenCollection tokens);

    // Main function to scan a file
    Result<ScanResult> scanFile(const string &filename);

    // Utility function to print tokens
    void printTokens(const TokenCollection &tokens);
}

#endif // SCANNER_H
