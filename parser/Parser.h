// //
// // Created by USER on 5/14/2025.
// //
//
// #ifndef PARSER_H
// #define PARSER_H
// #include <iostream>
// #include <vector>
// #include <string>
// #include <unordered_map>
// #include <fstream>
// #include <sstream>
// #include <regex>
// #include <set>
// #include "../common.h"
//
// using namespace std;
//
//
// class Parser {
//
// public:
//     vector<Token> tokens;
//     int errorCount = 0;
//
// private:
//     size_t current = 0;
//     set<string> validTypes = {
//         "Void", "Integer", "SInteger", "Character", "String", "Float", "SFloat"
//     };
//
//     // Custom type names that we've seen in the program
//     set<string> customTypes;
//
//
// Token peek();
//
// Token previous();
//
// bool check(const string& type);
//
// bool checkText(const string& text);
//
// bool match(const string& type);
//
// bool matchText(const string& text);
//
// void advance();
//
// void error(const string& message);
//
// void success(const string& rule, int lineNumber);
//
// void parseProgram();
// void parseDeclaration();
//
// void parseInclude();
//
// void parseStructDeclaration();
//
// void parseFunctionDeclaration(const string& returnType, const string& functionName, int startLine);
//
// bool parseVariableDeclaration();
//
// void parseStatement();
//
// void parseCompoundStatement();
//
// void parseSelectionStatement(int startLine);
//
// void parseIterationStatement(int startLine);
//
// void parseJumpStatement(int startLine);
//
// bool parseExpressionStatement();
//
// bool parseExpression();
//
//
// public:
// Parser(const vector<Token>& tokens) : tokens(tokens) {
//     // Initialize with some common types that might be used in the program
//     customTypes.insert("Person");
// }
//
// void parse();
//
// // Getter for error count
// int getErrorCount() const ;
//
// };
//
// #endif //PARSER_H
