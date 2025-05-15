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
    if (parseIdAssign())
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



bool Parser::parseTypeSpecifier() {

    if (matchText("Imw")) {
        return true;
    } else if (matchText("SIMw")) {
        return true;
    } else if (matchText("Chj")) {
        return true;
    } else if (matchText("Series")) {
        return true;
    } else if (matchText("IMwf")) {
        return true;
    } else if (matchText("SIMwf")) {
        return true;
    } else if (matchText("NOReturn")) {
        return true;
    }
    return false;
}
bool Parser::parseParams() {
    if (matchText("NOReturn")) return true;
    if (checkText(")")) return true; // ε
    return parseParamList();
}

bool Parser::parseParamList(){
    int startLine = peek().lineNumber;
    
    if (parseParam()) {
        while (matchText(",")) {
            if (!parseParam()) {
                error("Expected parameter after ','");
                return false;
            }
        }   
        success("param-list", startLine);
        return true;
    }
    return false;
}

bool Parser::parseParam(){
    int startLine = peek().lineNumber;
    if (parseTypeSpecifier()) {
        if (matchType("Identifier")) {
            success("param", startLine);
            return true;
        } else {
            error("Expected identifier after type specifier in parameter");
        }
    }
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
    return parseCall() || parseNum() || parseExpressionWithBraces() || parseIdAssign();
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
bool Parser::parseStatementList() {
    while (parseStatement());
    return true; // epsilon allowed
}
bool Parser::parseSelectionStatement() {
    if (!matchText("IfTrue")) return false;
    if (!matchText("(")) return false;
    if (!parseExpression()) return false;
    if (!matchText(")")) return false;
    if (!parseStatement()) return false;

    if (matchText("Otherwise")) {
        if (!parseStatement()) return false;
    }
    return true;
}
bool Parser::parseIterationStatement() {
    if (matchText("RepeatWhen")) {
        if (!matchText("(")) return false;
        if (!parseExpression()) return false;
        if (!matchText(")")) return false;
        if (!parseStatement()) return false;
        return true;
    }
    if (matchText("Reiterate")) {
        if (!matchText("(")) return false;
        if (!parseExpression()) return false;
        if (!matchText(";")) return false;
        if (!parseExpression()) return false;
        if (!matchText(";")) return false;
        if (!parseExpression()) return false;
        if (!matchText(")")) return false;
        if (!parseStatement()) return false;
        return true;
    }
    return false;
}
bool Parser::parseJumpStatement() {
    if (matchText("Turnback")) {
        if (!parseExpression()) return false;
        if (!matchText(";")) return false;
        return true;
    }
    if (matchText("Stop")) {
        if (!matchText(";")) return false;
        return true;
    }
    return false;
}
bool Parser::parseStatement() {
    if (parseExpressionStatement()) return true;
    if (parseCompoundStatement()) return true;
    if (parseSelectionStatement()) return true;
    if (parseIterationStatement()) return true;
    if (parseJumpStatement()) return true;
    return false;
}

bool Parser::parseCompoundStatement() {
    if (!matchText("{")) return false;
    parseComment(); // optional comment before declarations
    if (!parseLocalDeclarations()) return false;
    if (!parseStatementList()) return false;
    if (!matchText("}")) return false;
    return true;
}
bool Parser::parseVarDeclaration(){
    int startLine = peek().lineNumber;
    if (parseTypeSpecifier()) {
        if (matchType("Identifier")) {
            if (matchText(";")) {
                success("var-declaration", startLine);
                return true;
            } else {
                error("Expected ';' after variable declaration");
            }
        }
    }
    return false;
}
bool Parser::parseFunDeclaration() {
    if (parseComment()) {
        if (parseTypeSpecifier() && matchType("Identifier")) {
            success("fun-declaration (comment-style)", previous().lineNumber);
            return true;
        }
        return false;
    }

    if (parseTypeSpecifier() && matchType("Identifier") &&
        matchText("(") && parseParams() && matchText(")") &&
        parseCompoundStatement()) {
        success("fun-declaration", previous().lineNumber);
        return true;
        }

    return false;
}
bool Parser::parseLocalDeclarations() {
    while (parseVarDeclaration());
    return true; // epsilon
}
bool Parser::parseDeclaration() {
    return parseVarDeclaration() || parseFunDeclaration();
}
bool Parser::parseDeclarationList() {
    if (!parseDeclaration()) return false;
    while (parseDeclaration());
    return true;
}

bool Parser::parseProgram() {
    return parseDeclarationList() || parseComment() || parseIncludeCommand();
}


bool Parser::parseComment() {
    if (matchText("/@")) {
        while (!matchText("@/") && current < tokens.size()) {
            advance();
        }
        success("comment (multi-line)", previous().lineNumber);
        return true;
    }

    if (matchText("/^")) {
        // Consume rest of the line or until end
        while (current < tokens.size() && tokens[current].lineNumber == previous().lineNumber) {
            advance();
        }
        success("comment (single-line)", previous().lineNumber);
        return true;
    }

    return false;
}

bool Parser::parseIncludeCommand() {
    if (matchText("Include")) {
        if (matchText("(")) {
            if (parseFName()) {
                if (matchText(".txt")) {
                    if (matchText(")")) {
                        if (matchText(";")) {
                            success("include_command", previous().lineNumber);
                            return true;
                        } else {
                            error("Expected ';' after include command");
                        }
                    } else {
                        error("Expected ')' in include command");
                    }
                } else {
                    error("Expected '.txt' after filename in include command");
                }
            }
        } else {
            error("Expected '(' in include command");
        }
    }
    return false;
}

bool Parser::parseFName() {
    if (matchType("Identifier")) {
        success("F_name", previous().lineNumber);
        return true;
    }
    return false;
}




///////////////////////////


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

void Parser::parse() {
    if (!parseProgram()) {
        error("Invalid program structure");
    }

    if (current < tokens.size()) {
        error("Unexpected tokens after program end");
    }

    std::cout << "Total NO of errors: " << errorCount << std::endl;
}
// Getter for error count
int Parser::getErrorCount() const {
    return errorCount;
}




/////////////////////////////////

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



