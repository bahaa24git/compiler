//
// Created by USER on 5/13/2025.
//
#include "scanner/Scanner.h"
#include "parser/Parser.h"
int main() {
    Scanner scanner;
    vector<Token> tokens = scanner.scanFile("D:\\anas\\collage\\compiler\\compiler from khaled\\compiler\\old\\input.txt");
    scanner.printTokens();
    // Parser parser(tokens);
    // parser.parse();
    return 0;
}
//
//Line : 8 Token Text: , Token Type: Punctuation
//Line : 10 Token Text: ; Token Type: Punctuation

//cout << "Line : " << token.lineNumber << " Error in Token Text: " << token.text << " Token Type: Invalid Identifier" << endl;

/*
/@
not closed
*/