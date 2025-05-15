//
// Created by USER on 5/13/2025.
//
#include "scanner/Scanner.h"
#include "parser/Parser.h"
int main() {
    cout << "Scanner Output:\n";
    Scanner scanner;
    vector<Token> tokens = scanner.scanFile("D:\\anas\\collage\\compiler\\compiler from khaled\\compiler\\old\\input.txt");
    scanner.printTokens();
    cout << endl << endl;
    cout << "Parser Output:\n";
    Parser parser(tokens);
    parser.parse();
    return 0;
}

//cout << "Line : " << token.lineNumber << " Error in Token Text: " << token.text << " Token Type: Invalid Identifier" << endl;

/*
/@
not closed
*/