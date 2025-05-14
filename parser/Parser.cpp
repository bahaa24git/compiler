//
// Created by USER on 5/14/2025.
//

#include "Parser.h"
Token Parser::peek() {
    if (current >= tokens.size()) {
        return {-1, "", "EOF"};
    }
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::checkType(const string& type) {
    if (current >= tokens.size()) return false;
    return tokens[current].type == type;
}

bool Parser::checkText(const string& text) {
    if (current >= tokens.size()) return false;
    return tokens[current].text == text;
}

bool Parser::matchType(const string& type) {
    if (checkType(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchText(const string& text) {
    if (checkText(text)) {
        advance();
        return true;
    }
    return false;
}

void Parser::advance() {
    if (current < tokens.size()) {
        current++;
    }
}

void Parser::error(const string& message) {
    Token token = peek();
    cerr << "Line : " << token.lineNumber << " Not Matched - Error: " << message;
    if (token.lineNumber != -1) {
        cerr << " (found '" << token.text << "')";
    }
    cerr << endl;
    errorCount++;
}

void Parser::success(const string& rule, int lineNumber) {
    cout << "Line : " << lineNumber << " Matched Rule used: " << rule << endl;
}





///////////////////////////////14, 18-34 and 35. value → INT_NUM | FLOAT_NUM we only have constant
bool Parser::parseRelOp() {
    if( matchText("<=") || matchText(">=") ||
        matchText("==") || matchText("!=") ||
        matchText("<") || matchText(">") ||
        matchText("&&") || matchText("||") )
        return true;
    return false;
}
bool Parser::parseSimpleExpression() {
    if(!parseAdditiveExpression())
        return false;
    while (current < tokens.size() && parseRelOp())
    {
        if (!parseAdditiveExpression()) return false;
    }
    return true;
}
bool Parser::parseIdAssign() {
    if (matchType("Identifier"))
        return true;
    return false;
}
bool Parser::parseArgList() {
    if(!parseExpression())
        return false;
    while (current < tokens.size() && matchText(","))
    {
        if (!parseExpression()) return false;
    }
    return true;
}
bool Parser::parseArgs() {
    if(parseArgList())
        return true;
    // epsilon production (empty args)
    return true;
}
bool Parser::parseCall() {
    if (matchType("Identifier"))
        if(matchText("("))
            if(parseArgs())
            if(matchText(")"))
                return true;
    return false;
}
bool Parser::parseValue() {//INT_NUM | FLOAT_NUM ???? 35
    if(matchType("Constant"))
        return true;
    return false;
}
bool Parser::parsePosNum() {
    if(matchText("+"))
        if(parseValue())
            return true;
    return false;
}
bool Parser::parseNegNum() {
    if(matchText("-"))
        if(parseValue())
            return true;
    return false;
}
bool Parser::parseSignedNum() {
    if(parsePosNum())
        return true;
    if(parseNegNum())
        return true;
    return false;
}
bool Parser::parseUnsignedNum() {
    if(parseValue())
        return true;
    return false;
}
bool Parser::parseNum() {
    if(parseSignedNum())
            return true;
    if(parseUnsignedNum())
            return true;
    return false;
}
bool Parser::parseFactor() {
    if ( parseIdAssign() || parseNum() || parseExpressionWithBraces() || parseCall()) {
        return true;
    }
    return false;
}
bool Parser::parseMulOp() {
    if(!(matchText("*") || matchText("/")))
        return false;
    return true;
}
bool Parser::parseTerm() {
    if (!parseFactor()) return false;

    while (current < tokens.size() && parseMulOp())
    {
        if (!parseFactor()) return false;
    }
    return true;
}
bool Parser::parseAddOp() {
    if(!(matchText("+") || matchText("-")))
        return false;
    return true;
}
bool Parser::parseAdditiveExpression() {
    if (!parseTerm()) return false;

    while (current < tokens.size() && parseAddOp())
    {
        if (!parseTerm()) return false;
    }
    return true;
}

bool Parser::parseExpression() {//this an important one
    if (parseIdAssign()) {
        if (matchType("Assignment operator")) {
            return parseExpression();
        }
        return true;
    }
    return parseSimpleExpression();
}
bool Parser::parseExpressionWithBraces() {
    if (matchText("("))
        if(parseExpression())
            if (matchText(")"))
                return true;
    return false;

}
bool Parser::parseExpressionStatement() {
    if(parseExpression()) {
        if (matchText(";")) {
            cout << "Line: " << tokens[current].lineNumber << " Matched Rule: expression-statement\n";
            return true;
        }
    }
    else if (matchText(";")) {
        cout << "Line: " << tokens[current].lineNumber << " Matched Rule: expression-statement\n";
        return true;
    }
    return false;
}


///////////////////////////


// void Parser::parseDeclaration() {
//     int startLine = peek().lineNumber;
//
//     if (checkType("Inclusion")) {
//         parseInclude();
//         return;
//     }
//
//     // Check for struct declaration
//     if (matchText("struct")) {
//         parseStructDeclaration();
//         return;
//     }
//
//     // Check for function declaration
//     if (checkType("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//         string returnType = peek().text;
//         advance(); // Consume the return type
//
//         if (matchType("Identifier")) {
//             string functionName = tokens[current-1].text;
//
//             if (matchText("(")) {
//                 // This is a function declaration
//                 parseFunctionDeclaration(returnType, functionName, startLine);
//                 return;
//             } else {
//                 // Not a function declaration, reset position
//                 current -= 2;
//             }
//         } else {
//             // Not a function declaration, reset position
//             current -= 1;
//         }
//     }
//
//     // Special handling for struct variable declarations
//     if (checkType("Identifier")) {
//         // Save current position to allow backtracking
//         size_t savedPos = current;
//         string typeName = peek().text;
//
//         // Check if it's a known struct type
//         if (customTypes.find(typeName) != customTypes.end()) {
//             advance(); // Consume the type name
//
//             if (matchType("Identifier")) {
//                 string varName = tokens[current-1].text;
//
//                 // Register this variable name as a struct instance
//                 customTypes.insert(varName);
//
//                 // This is a struct variable declaration
//                 if (matchText(";")) {
//                     success("variable-declaration", startLine);
//                     return;
//                 } else {
//                     // Try to recover by looking for the next semicolon
//                     while (!checkType("EOF") && !checkText(";")) {
//                         advance();
//                     }
//                     if (matchText(";")) {
//                         success("variable-declaration", startLine);
//                         return;
//                     } else {
//                         error("Expected ';' after struct variable declaration");
//                         return;
//                     }
//                 }
//             }
//         }
//
//         // Not a struct variable declaration, reset position
//         current = savedPos;
//     }
//
//     // Try to parse a variable declaration
//     if (parseVariableDeclaration()) {
//         return;
//     }
//
//     // Try to parse a statement
//     parseStatement();
// }
//
// void Parser::parseProgram() {
//     while (!checkType("EOF")) {
//         parseDeclaration();
//     }
// }
// void Parser::parse() {
//     try {
//         parseProgram();
//     } catch (const std::exception& e) {
//         // Catch any exceptions and continue parsing
//         cerr << "Error during parsing: " << e.what() << endl;
//     }
//
//     // Print all errors found during parsing
//     cout << "Total NO of errors: " << errorCount << endl;
// }
//
// // Getter for error count
// int Parser::getErrorCount() const {
//     return errorCount;
// }

