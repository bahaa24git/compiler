// //
// // Created by USER on 5/14/2025.
// //
//
// #include "Parser.h"
// Token Parser::peek() {
//     if (current >= tokens.size()) {
//         return {-1, "", "EOF"};
//     }
//     return tokens[current];
// }
//
// Token Parser::previous() {
//     return tokens[current - 1];
// }
//
// bool Parser::check(const string& type) {
//     if (current >= tokens.size()) return false;
//     return tokens[current].type == type;
// }
//
// bool Parser::checkText(const string& text) {
//     if (current >= tokens.size()) return false;
//     return tokens[current].text == text;
// }
//
// bool Parser::match(const string& type) {
//     if (check(type)) {
//         advance();
//         return true;
//     }
//     return false;
// }
//
// bool Parser::matchText(const string& text) {
//     if (checkText(text)) {
//         advance();
//         return true;
//     }
//     return false;
// }
//
// void Parser::advance() {
//     if (current < tokens.size()) {
//         current++;
//     }
// }
//
// void Parser::error(const string& message) {
//     Token token = peek();
//     cerr << "Line : " << token.lineNumber << " Not Matched - Error: " << message;
//     if (token.lineNumber != -1) {
//         cerr << " (found '" << token.text << "')";
//     }
//     cerr << endl;
//     errorCount++;
// }
//
// void Parser::success(const string& rule, int lineNumber) {
//     cout << "Line : " << lineNumber << " Matched Rule used: " << rule << endl;
// }
//
// void Parser::parseProgram() {
//     while (!check("EOF")) {
//         parseDeclaration();
//     }
// }
//
//
// void Parser::parseDeclaration() {
//     int startLine = peek().lineNumber;
//
//     if (check("Inclusion")) {
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
//     if (check("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//         string returnType = peek().text;
//         advance(); // Consume the return type
//
//         if (match("Identifier")) {
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
//     if (check("Identifier")) {
//         // Save current position to allow backtracking
//         size_t savedPos = current;
//         string typeName = peek().text;
//
//         // Check if it's a known struct type
//         if (customTypes.find(typeName) != customTypes.end()) {
//             advance(); // Consume the type name
//
//             if (match("Identifier")) {
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
//                     while (!check("EOF") && !checkText(";")) {
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
// void Parser::parseInclude() {
//     int startLine = peek().lineNumber;
//     advance(); // Consume the Inclusion token
//     success("include-directive", startLine);
// }
//
// void Parser::parseStructDeclaration() {
//     int startLine = peek().lineNumber;
//
//     if (match("Identifier")) {
//         string structName = tokens[current-1].text;
//
//         // Add this struct name to our custom types
//         customTypes.insert(structName);
//
//         if (matchText("{")) {
//             // Parse struct members
//             while (!check("EOF") && !checkText("}")) {
//                 // Skip any comments
//                 if (check("Comment")) {
//                     advance();
//                     continue;
//                 }
//
//                 // Parse struct member declaration
//                 if (check("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//                     string memberType = peek().text;
//                     advance(); // Consume the member type
//
//                     if (match("Identifier")) {
//                         string memberName = tokens[current-1].text;
//
//                         if (matchText(";")) {
//                             // Successfully parsed a struct member
//                         } else {
//                             error("Expected ';' after struct member declaration");
//                             // Try to recover by skipping to the next semicolon or closing brace
//                             while (!check("EOF") && !checkText(";") && !checkText("}")) {
//                                 advance();
//                             }
//                             if (checkText(";")) {
//                                 advance(); // Consume the semicolon
//                             }
//                         }
//                     } else {
//                         error("Expected identifier for struct member name");
//                         // Try to recover by skipping to the next semicolon or closing brace
//                         while (!check("EOF") && !checkText(";") && !checkText("}")) {
//                             advance();
//                         }
//                         if (checkText(";")) {
//                             advance(); // Consume the semicolon
//                         }
//                     }
//                 } else {
//                     error("Expected valid type for struct member");
//                     // Try to recover by skipping to the next semicolon or closing brace
//                     while (!check("EOF") && !checkText(";") && !checkText("}")) {
//                         advance();
//                     }
//                     if (checkText(";")) {
//                         advance(); // Consume the semicolon
//                     }
//                 }
//             }
//
//             if (matchText("}")) {
//                 if (matchText(";")) {
//                     success("struct-declaration", startLine);
//                     return;
//                 } else {
//                     error("Expected ';' after struct declaration");
//                     return;
//                 }
//             } else {
//                 error("Expected '}' to close struct declaration");
//                 return;
//             }
//         } else {
//             error("Expected '{' after struct name");
//             return;
//         }
//     } else {
//         error("Expected identifier for struct name");
//         return;
//     }
// }
//
// void Parser::parseFunctionDeclaration(const string& returnType, const string& functionName, int startLine) {
//     // Parse parameter list
//     while (!check("EOF") && !checkText(")")) {
//         // Skip any comments
//         if (check("Comment")) {
//             advance();
//             continue;
//         }
//
//         // Parse parameter
//         if (check("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//             string paramType = peek().text;
//             advance(); // Consume the parameter type
//
//             if (match("Identifier")) {
//                 string paramName = tokens[current-1].text;
//
//                 // Check for more parameters
//                 if (matchText(",")) {
//                     // Continue to the next parameter
//                 } else if (checkText(")")) {
//                     // End of parameter list
//                     break;
//                 } else {
//                     error("Expected ',' or ')' after function parameter");
//                     // Try to recover by skipping to the next comma or closing parenthesis
//                     while (!check("EOF") && !checkText(",") && !checkText(")")) {
//                         advance();
//                     }
//                 }
//             } else {
//                 error("Expected identifier for parameter name");
//                 // Try to recover by skipping to the next comma or closing parenthesis
//                 while (!check("EOF") && !checkText(",") && !checkText(")")) {
//                     advance();
//                 }
//             }
//         } else if (checkText(")")) {
//             // Empty parameter list or end of parameters
//             break;
//         } else {
//             error("Expected valid type for parameter");
//             // Try to recover by skipping to the next comma or closing parenthesis
//             while (!check("EOF") && !checkText(",") && !checkText(")")) {
//                 advance();
//             }
//         }
//     }
//
//     if (matchText(")")) {
//         // Parse function body
//         if (checkText("{")) {
//             parseCompoundStatement();
//             success("function-declaration", startLine);
//             return;
//         } else {
//             error("Expected compound statement for function body");
//             return;
//         }
//     } else {
//         error("Expected ')' to close parameter list");
//         return;
//     }
// }
//
// bool Parser::parseVariableDeclaration() {
//     int startLine = peek().lineNumber;
//
//     // Check if it's a valid type
//     if (check("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//         string varType = peek().text;
//         advance(); // Consume the type
//
//         if (match("Identifier")) {
//             string varName = tokens[current-1].text;
//
//             // Check for initialization
//             if (matchText("=")) {
//                 // Parse initializer
//                 if (match("Constant") || match("String Literal") || match("Character Literal") || parseExpression()) {
//                     // Successfully parsed initializer
//                 } else {
//                     error("Expected expression for variable initialization");
//                     // Try to recover by skipping to the next semicolon
//                     while (!check("EOF") && !checkText(";")) {
//                         advance();
//                     }
//                 }
//             }
//
//             if (matchText(";")) {
//                 success("variable-declaration", startLine);
//                 return true;
//             } else {
//                 error("Expected ';' after variable declaration");
//                 // Try to recover by skipping to the next semicolon
//                 while (!check("EOF") && !checkText(";")) {
//                     advance();
//                 }
//                 if (matchText(";")) {
//                     success("variable-declaration", startLine);
//                     return true;
//                 }
//                 return false;
//             }
//         } else {
//             // Not a variable declaration, reset position
//             current -= 1;
//             return false;
//         }
//     }
//
//     return false;
// }
//
// void Parser::parseStatement() {
//     int startLine = peek().lineNumber;
//
//     // Skip comments
//     if (check("Comment")) {
//         advance();
//         return;
//     }
//
//     // Check for compound statement
//     if (checkText("{")) {
//         parseCompoundStatement();
//         return;
//     }
//
//     // Check for selection statement (if-else)
//     if (matchText("if")) {
//         parseSelectionStatement(startLine);
//         return;
//     }
//
//     // Check for iteration statement (while)
//     if (matchText("while")) {
//         parseIterationStatement(startLine);
//         return;
//     }
//
//     // Check for jump statement (return)
//     if (matchText("return")) {
//         parseJumpStatement(startLine);
//         return;
//     }
//
//     // Check for jump statement (OutLoop)
//     if (matchText("OutLoop")) {
//         if (matchText(";")) {
//             success("jump-statement", startLine);
//             return;
//         } else {
//             error("Expected ';' after OutLoop statement");
//             return;
//         }
//     }
//
//     // Check for expression statement
//     if (parseExpressionStatement()) {
//         return;
//     }
//
//     // If we get here, we couldn't parse the statement
//     error("Invalid statement");
//     // Try to recover by skipping to the next semicolon
//     while (!check("EOF") && !checkText(";")) {
//         advance();
//     }
//     if (matchText(";")) {
//         // Successfully recovered
//     }
// }
//
// void Parser::parseCompoundStatement() {
//     int startLine = peek().lineNumber;
//
//     if (matchText("{")) {
//         // Parse statements
//         while (!check("EOF") && !checkText("}")) {
//             parseDeclaration();
//         }
//
//         if (matchText("}")) {
//             success("compound-statement", startLine);
//             return;
//         } else {
//             error("Expected '}' to close compound statement");
//             return;
//         }
//     } else {
//         error("Expected '{' to begin compound statement");
//         return;
//     }
// }
//
// void Parser::parseSelectionStatement(int startLine) {
//     if (matchText("(")) {
//         // Parse condition
//         if (parseExpression()) {
//             if (matchText(")")) {
//                 // Parse then-statement
//                 parseStatement();
//
//                 // Check for else-statement
//                 if (matchText("else")) {
//                     parseStatement();
//                 }
//
//                 success("selection-statement", startLine);
//                 return;
//             } else {
//                 error("Expected ')' after if condition");
//                 return;
//             }
//         } else {
//             error("Expected expression for if condition");
//             return;
//         }
//     } else {
//         error("Expected '(' after if");
//         return;
//     }
// }
//
// void Parser::parseIterationStatement(int startLine) {
//     if (matchText("(")) {
//         // Parse condition
//         if (parseExpression()) {
//             if (matchText(")")) {
//                 // Parse body
//                 parseStatement();
//
//                 success("iteration-statement", startLine);
//                 return;
//             } else {
//                 error("Expected ')' after while condition");
//                 return;
//             }
//         } else {
//             error("Expected expression for while condition");
//             return;
//         }
//     } else {
//         error("Expected '(' after while");
//         return;
//     }
// }
//
// void Parser::parseJumpStatement(int startLine) {
//     // Parse optional return value
//     if (!checkText(";")) {
//         if (!parseExpression()) {
//             error("Expected expression or ';' after return");
//             return;
//         }
//     }
//
//     if (matchText(";")) {
//         success("jump-statement", startLine);
//         return;
//     } else {
//         error("Expected ';' after return statement");
//         return;
//     }
// }
//
// bool Parser::parseExpressionStatement() {
//     int startLine = peek().lineNumber;
//
//     // Check for empty statement
//     if (matchText(";")) {
//         success("expression-statement", startLine);
//         return true;
//     }
//
//     // Try to parse an expression
//     if (parseExpression()) {
//         if (matchText(";")) {
//             success("expression-statement", startLine);
//             return true;
//         } else {
//             error("Expected ';' after expression");
//             // Try to recover by skipping to the next semicolon
//             while (!check("EOF") && !checkText(";")) {
//                 advance();
//             }
//             if (matchText(";")) {
//                 success("expression-statement", startLine);
//                 return true;
//             }
//             return false;
//         }
//     }
//
//     return false;
// }
//
// bool Parser::parseExpression() {
//     // Check for identifier (variable, function call, etc.)
//     if (match("Identifier")) {
//         string identifierName = tokens[current-1].text;
//
//         // Handle struct member access
//         if (match("Access Operator")) {
//             if (match("Identifier")) {
//                 // We have a valid struct member access
//                 if (match("Assignment operator")) {
//                     // It's an assignment to a struct member
//                     if (match("Constant")) {
//                         // Check if this is a floating-point literal
//                         if (checkText(".")) {
//                             matchText("."); // Consume the decimal point
//                             if (match("Constant")) {
//                                 // Successfully parsed a floating-point literal
//                             }
//                         }
//                         // Simple constant assignment
//                         return true;
//                     } else if (match("String Literal") || match("Character Literal")) {
//                         // String or character literal assignment
//                         return true;
//                     } else if (parseExpression()) {
//                         return true;
//                     } else {
//                         error("Invalid expression in struct member assignment");
//                         return false;
//                     }
//                 } else {
//                     // It's a struct member access without assignment
//                     return true;
//                 }
//             } else {
//                 error("Expected identifier after '->' operator");
//                 return false;
//             }
//         }
//         // Handle array indexing
//         else if (matchText("[")) {
//             // Parse array index
//             if (match("Constant") || match("Identifier") || parseExpression()) {
//                 if (matchText("]")) {
//                     // Valid array access
//                     if (match("Assignment operator")) {
//                         // It's an assignment to an array element
//                         if (match("Constant") || match("String Literal") || match("Character Literal") || parseExpression()) {
//                             return true;
//                         } else {
//                             error("Invalid expression in array element assignment");
//                             return false;
//                         }
//                     } else {
//                         // It's an array access without assignment
//                         return true;
//                     }
//                 } else {
//                     error("Expected ']' after array index");
//                     return false;
//                 }
//             } else {
//                 error("Invalid array index expression");
//                 return false;
//             }
//         }
//         // Handle function call
//         else if (matchText("(")) {
//             // Parse argument list
//             if (!checkText(")")) {
//                 do {
//                     if (checkText(")")) {
//                         break; // Empty argument list or end of arguments
//                     }
//
//                     if (!parseExpression()) {
//                         error("Invalid function argument");
//                         return false;
//                     }
//                 } while (matchText(","));
//             }
//
//             if (matchText(")")) {
//                 return true;
//             } else {
//                 error("Expected ')' to close function call");
//                 return false;
//             }
//         }
//         // Handle assignment
//         else if (match("Assignment operator")) {
//             if (match("Constant") || match("String Literal") || match("Character Literal") || parseExpression()) {
//                 return true;
//             } else {
//                 error("Invalid expression in assignment");
//                 return false;
//             }
//         }
//         // Simple variable reference
//         else {
//             return true;
//         }
//     }
//     // Check for constant
//     else if (match("Constant")) {
//         // Check if this is a floating-point literal
//         if (checkText(".")) {
//             matchText("."); // Consume the decimal point
//             if (match("Constant")) {
//                 // Successfully parsed a floating-point literal
//             }
//         }
//         return true;
//     }
//     // Check for string literal
//     else if (match("String Literal")) {
//         return true;
//     }
//     // Check for character literal
//     else if (match("Character Literal")) {
//         return true;
//     }
//     // Check for parenthesized expression
//     else if (matchText("(")) {
//         if (parseExpression()) {
//             if (matchText(")")) {
//                 return true;
//             } else {
//                 error("Expected ')' to close expression");
//                 return false;
//             }
//         } else {
//             error("Invalid expression in parentheses");
//             return false;
//         }
//     }
//
//     return false;
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
//
//
// // vector<Token> loadTokensFromFile(const string& filename) {
// //     vector<Token> tokens;
// //     ifstream file(filename);
// //     string line;
// //     while (getline(file, line)) {
// //         smatch match;
// //         if (regex_search(line, match, regex(R"(Line\s*:\s*(\d+).*Token Text: (.*?)\s+Token Type: (.*))"))) {
// //             tokens.push_back({stoi(match[1]), match[2], match[3]});
// //         } else if (regex_search(line, match, regex(R"(Line\s*:\s*(\d+).*Error in Token Text: (.*?)\s+Token Type: Invalid Identifier)"))) {
// //             tokens.push_back({stoi(match[1]), match[2], "Invalid Identifier"});
// //         }
// //     }
// //     return tokens;
// // }