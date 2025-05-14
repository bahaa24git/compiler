//
// Created by USER on 5/13/2025.
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


class Scanner {

public:
    Scanner() {
        initKeywords();
    }
    vector<Token> tokens;
    int errorCount = 0;
private:
    unordered_map<string, string> keywords;

    bool inMultiLineComment = false;
    set<string> processedFiles; // To prevent recursive inclusion loops

    void initKeywords();
    bool isValidIdentifier(const string &token);
    bool isNumber(const string &token);
    void addToken(int lineNumber, const string &text, const string &type);

    // Tokenize a line character by character for better handling of special characters
    // Process a token and determine its type
    void processToken(const string &token, int lineNumber);
    void tokenizeLine(const string &line, int lineNumber);
    void handleInclude(const string& line, const string& currentFile, int lineNumber);
    void scanLine(string line, const string& filename, int lineNumber);
public:
    vector<Token> scanFile(const string &filename);
    void printTokens() const;
};



#endif //SCANNER_H
