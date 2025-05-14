// // project3_parser_test.cpp
// // Modified parser for testing error handling
//
// #include <iostream>
// #include <vector>
// #include <string>
// #include <unordered_map>
// #include <fstream>
// #include <sstream>
// #include <regex>
// #include <set>
//
// using namespace std;
//
// struct Token {
//     int lineNumber;
//     string text;
//     string type;
// };
//
// class Parser {
// private:
//     vector<Token> tokens;
//     size_t current = 0;
//     int errorCount = 0;
//     set<string> validTypes = {
//         "Void", "Integer", "SInteger", "Character", "String", "Float", "SFloat"
//     };
//
//     // Custom type names that we've seen in the program
//     set<string> customTypes;
//
//     Token peek() {
//         if (current >= tokens.size()) {
//             return {-1, "", "EOF"};
//         }
//         return tokens[current];
//     }
//
//     Token previous() {
//         return tokens[current - 1];
//     }
//
//     bool check(const string& type) {
//         if (current >= tokens.size()) return false;
//         return tokens[current].type == type;
//     }
//
//     bool checkText(const string& text) {
//         if (current >= tokens.size()) return false;
//         return tokens[current].text == text;
//     }
//
//     bool match(const string& type) {
//         if (check(type)) {
//             advance();
//             return true;
//         }
//         return false;
//     }
//
//     bool matchText(const string& text) {
//         if (checkText(text)) {
//             advance();
//             return true;
//         }
//         return false;
//     }
//
//     void advance() {
//         if (current < tokens.size()) {
//             current++;
//         }
//     }
//
//     void error(const string& message) {
//         Token token = peek();
//         cerr << "Line : " << token.lineNumber << " Not Matched - Error: " << message;
//         if (token.lineNumber != -1) {
//             cerr << " (found '" << token.text << "')";
//         }
//         cerr << endl;
//         errorCount++;
//     }
//
//     void success(const string& rule, int lineNumber) {
//         cout << "Line : " << lineNumber << " Matched Rule used: " << rule << endl;
//     }
//
//     void parseProgram() {
//         while (!check("EOF")) {
//             parseDeclaration();
//         }
//     }
//
//     void parseDeclaration() {
//         int startLine = peek().lineNumber;
//
//         if (check("Inclusion")) {
//             parseInclude();
//             return;
//         }
//
//         // Check for struct declaration
//         if (matchText("struct")) {
//             parseStructDeclaration();
//             return;
//         }
//
//         // Check for function declaration
//         if (check("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//             string returnType = peek().text;
//             advance(); // Consume the return type
//
//             if (match("Identifier")) {
//                 string functionName = tokens[current-1].text;
//
//                 if (matchText("(")) {
//                     // This is a function declaration
//                     parseFunctionDeclaration(returnType, functionName, startLine);
//                     return;
//                 } else {
//                     // Not a function declaration, reset position
//                     current -= 2;
//                 }
//             } else {
//                 // Not a function declaration, reset position
//                 current -= 1;
//             }
//         }
//
//         // Special handling for struct variable declarations
//         if (check("Identifier")) {
//             // Save current position to allow backtracking
//             size_t savedPos = current;
//             string typeName = peek().text;
//
//             // Check if it's a known struct type
//             if (customTypes.find(typeName) != customTypes.end()) {
//                 advance(); // Consume the type name
//
//                 if (match("Identifier")) {
//                     string varName = tokens[current-1].text;
//
//                     // Register this variable name as a struct instance
//                     customTypes.insert(varName);
//
//                     // This is a struct variable declaration
//                     if (matchText(";")) {
//                         success("variable-declaration", startLine);
//                         return;
//                     } else {
//                         // Try to recover by looking for the next semicolon
//                         while (!check("EOF") && !checkText(";")) {
//                             advance();
//                         }
//                         if (matchText(";")) {
//                             success("variable-declaration", startLine);
//                             return;
//                         } else {
//                             error("Expected ';' after struct variable declaration");
//                             return;
//                         }
//                     }
//                 }
//             }
//
//             // Not a struct variable declaration, reset position
//             current = savedPos;
//         }
//
//         // Try to parse a variable declaration
//         if (parseVariableDeclaration()) {
//             return;
//         }
//
//         // Try to parse a statement
//         parseStatement();
//     }
//
//     void parseInclude() {
//         int startLine = peek().lineNumber;
//         advance(); // Consume the Inclusion token
//         success("include-directive", startLine);
//     }
//
//     void parseStructDeclaration() {
//         int startLine = peek().lineNumber;
//
//         if (match("Identifier")) {
//             string structName = tokens[current-1].text;
//
//             // Add this struct name to our custom types
//             customTypes.insert(structName);
//
//             if (matchText("{")) {
//                 // Parse struct members
//                 while (!check("EOF") && !checkText("}")) {
//                     // Skip any comments
//                     if (check("Comment")) {
//                         advance();
//                         continue;
//                     }
//
//                     // Parse struct member declaration
//                     if (check("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//                         string memberType = peek().text;
//                         advance(); // Consume the member type
//
//                         if (match("Identifier")) {
//                             string memberName = tokens[current-1].text;
//
//                             if (matchText(";")) {
//                                 // Successfully parsed a struct member
//                             } else {
//                                 error("Expected ';' after struct member declaration");
//                                 // Try to recover by skipping to the next semicolon or closing brace
//                                 while (!check("EOF") && !checkText(";") && !checkText("}")) {
//                                     advance();
//                                 }
//                                 if (checkText(";")) {
//                                     advance(); // Consume the semicolon
//                                 }
//                             }
//                         } else {
//                             error("Expected identifier for struct member name");
//                             // Try to recover by skipping to the next semicolon or closing brace
//                             while (!check("EOF") && !checkText(";") && !checkText("}")) {
//                                 advance();
//                             }
//                             if (checkText(";")) {
//                                 advance(); // Consume the semicolon
//                             }
//                         }
//                     } else {
//                         error("Expected valid type for struct member");
//                         // Try to recover by skipping to the next semicolon or closing brace
//                         while (!check("EOF") && !checkText(";") && !checkText("}")) {
//                             advance();
//                         }
//                         if (checkText(";")) {
//                             advance(); // Consume the semicolon
//                         }
//                     }
//                 }
//
//                 if (matchText("}")) {
//                     if (matchText(";")) {
//                         success("struct-declaration", startLine);
//                         return;
//                     } else {
//                         error("Expected ';' after struct declaration");
//                         return;
//                     }
//                 } else {
//                     error("Expected '}' to close struct declaration");
//                     return;
//                 }
//             } else {
//                 error("Expected '{' after struct name");
//                 return;
//             }
//         } else {
//             error("Expected identifier for struct name");
//             return;
//         }
//     }
//
//     void parseFunctionDeclaration(const string& returnType, const string& functionName, int startLine) {
//         // Parse parameter list
//         while (!check("EOF") && !checkText(")")) {
//             // Skip any comments
//             if (check("Comment")) {
//                 advance();
//                 continue;
//             }
//
//             // Parse parameter
//             if (check("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//                 string paramType = peek().text;
//                 advance(); // Consume the parameter type
//
//                 if (match("Identifier")) {
//                     string paramName = tokens[current-1].text;
//
//                     // Check for more parameters
//                     if (matchText(",")) {
//                         // Continue to the next parameter
//                     } else if (checkText(")")) {
//                         // End of parameter list
//                         break;
//                     } else {
//                         error("Expected ',' or ')' after function parameter");
//                         // Try to recover by skipping to the next comma or closing parenthesis
//                         while (!check("EOF") && !checkText(",") && !checkText(")")) {
//                             advance();
//                         }
//                     }
//                 } else {
//                     error("Expected identifier for parameter name");
//                     // Try to recover by skipping to the next comma or closing parenthesis
//                     while (!check("EOF") && !checkText(",") && !checkText(")")) {
//                         advance();
//                     }
//                 }
//             } else if (checkText(")")) {
//                 // Empty parameter list or end of parameters
//                 break;
//             } else {
//                 error("Expected valid type for parameter");
//                 // Try to recover by skipping to the next comma or closing parenthesis
//                 while (!check("EOF") && !checkText(",") && !checkText(")")) {
//                     advance();
//                 }
//             }
//         }
//
//         if (matchText(")")) {
//             // Parse function body
//             if (checkText("{")) {
//                 parseCompoundStatement();
//                 success("function-declaration", startLine);
//                 return;
//             } else {
//                 error("Expected compound statement for function body");
//                 return;
//             }
//         } else {
//             error("Expected ')' to close parameter list");
//             return;
//         }
//     }
//
//     bool parseVariableDeclaration() {
//         int startLine = peek().lineNumber;
//
//         // Check if it's a valid type
//         if (check("Identifier") && validTypes.find(peek().text) != validTypes.end()) {
//             string varType = peek().text;
//             advance(); // Consume the type
//
//             if (match("Identifier")) {
//                 string varName = tokens[current-1].text;
//
//                 // Check for initialization
//                 if (matchText("=")) {
//                     // Parse initializer
//                     if (match("Constant") || match("String Literal") || match("Character Literal") || parseExpression()) {
//                         // Successfully parsed initializer
//                     } else {
//                         error("Expected expression for variable initialization");
//                         // Try to recover by skipping to the next semicolon
//                         while (!check("EOF") && !checkText(";")) {
//                             advance();
//                         }
//                     }
//                 }
//
//                 if (matchText(";")) {
//                     success("variable-declaration", startLine);
//                     return true;
//                 } else {
//                     error("Expected ';' after variable declaration");
//                     // Try to recover by skipping to the next semicolon
//                     while (!check("EOF") && !checkText(";")) {
//                         advance();
//                     }
//                     if (matchText(";")) {
//                         success("variable-declaration", startLine);
//                         return true;
//                     }
//                     return false;
//                 }
//             } else {
//                 // Not a variable declaration, reset position
//                 current -= 1;
//                 return false;
//             }
//         }
//
//         return false;
//     }
//
//     void parseStatement() {
//         int startLine = peek().lineNumber;
//
//         // Skip comments
//         if (check("Comment")) {
//             advance();
//             return;
//         }
//
//         // Check for compound statement
//         if (checkText("{")) {
//             parseCompoundStatement();
//             return;
//         }
//
//         // Check for selection statement (if-else)
//         if (matchText("if")) {
//             parseSelectionStatement(startLine);
//             return;
//         }
//
//         // Check for iteration statement (while)
//         if (matchText("while")) {
//             parseIterationStatement(startLine);
//             return;
//         }
//
//         // Check for jump statement (return)
//         if (matchText("return")) {
//             parseJumpStatement(startLine);
//             return;
//         }
//
//         // Check for jump statement (OutLoop)
//         if (matchText("OutLoop")) {
//             if (matchText(";")) {
//                 success("jump-statement", startLine);
//                 return;
//             } else {
//                 error("Expected ';' after OutLoop statement");
//                 return;
//             }
//         }
//
//         // Check for expression statement
//         if (parseExpressionStatement()) {
//             return;
//         }
//
//         // If we get here, we couldn't parse the statement
//         error("Invalid statement");
//         // Try to recover by skipping to the next semicolon
//         while (!check("EOF") && !checkText(";")) {
//             advance();
//         }
//         if (matchText(";")) {
//             // Successfully recovered
//         }
//     }
//
//     void parseCompoundStatement() {
//         int startLine = peek().lineNumber;
//
//         if (matchText("{")) {
//             // Parse statements
//             while (!check("EOF") && !checkText("}")) {
//                 parseDeclaration();
//             }
//
//             if (matchText("}")) {
//                 success("compound-statement", startLine);
//                 return;
//             } else {
//                 error("Expected '}' to close compound statement");
//                 return;
//             }
//         } else {
//             error("Expected '{' to begin compound statement");
//             return;
//         }
//     }
//
//     void parseSelectionStatement(int startLine) {
//         if (matchText("(")) {
//             // Parse condition
//             if (parseExpression()) {
//                 if (matchText(")")) {
//                     // Parse then-statement
//                     parseStatement();
//
//                     // Check for else-statement
//                     if (matchText("else")) {
//                         parseStatement();
//                     }
//
//                     success("selection-statement", startLine);
//                     return;
//                 } else {
//                     error("Expected ')' after if condition");
//                     return;
//                 }
//             } else {
//                 error("Expected expression for if condition");
//                 return;
//             }
//         } else {
//             error("Expected '(' after if");
//             return;
//         }
//     }
//
//     void parseIterationStatement(int startLine) {
//         if (matchText("(")) {
//             // Parse condition
//             if (parseExpression()) {
//                 if (matchText(")")) {
//                     // Parse body
//                     parseStatement();
//
//                     success("iteration-statement", startLine);
//                     return;
//                 } else {
//                     error("Expected ')' after while condition");
//                     return;
//                 }
//             } else {
//                 error("Expected expression for while condition");
//                 return;
//             }
//         } else {
//             error("Expected '(' after while");
//             return;
//         }
//     }
//
//     void parseJumpStatement(int startLine) {
//         // Parse optional return value
//         if (!checkText(";")) {
//             if (!parseExpression()) {
//                 error("Expected expression or ';' after return");
//                 return;
//             }
//         }
//
//         if (matchText(";")) {
//             success("jump-statement", startLine);
//             return;
//         } else {
//             error("Expected ';' after return statement");
//             return;
//         }
//     }
//
//     bool parseExpressionStatement() {
//         int startLine = peek().lineNumber;
//
//         // Check for empty statement
//         if (matchText(";")) {
//             success("expression-statement", startLine);
//             return true;
//         }
//
//         // Try to parse an expression
//         if (parseExpression()) {
//             if (matchText(";")) {
//                 success("expression-statement", startLine);
//                 return true;
//             } else {
//                 error("Expected ';' after expression");
//                 // Try to recover by skipping to the next semicolon
//                 while (!check("EOF") && !checkText(";")) {
//                     advance();
//                 }
//                 if (matchText(";")) {
//                     success("expression-statement", startLine);
//                     return true;
//                 }
//                 return false;
//             }
//         }
//
//         return false;
//     }
//
//     bool parseExpression() {
//         // Check for identifier (variable, function call, etc.)
//         if (match("Identifier")) {
//             string identifierName = tokens[current-1].text;
//
//             // Handle struct member access
//             if (match("Access Operator")) {
//                 if (match("Identifier")) {
//                     // We have a valid struct member access
//                     if (match("Assignment operator")) {
//                         // It's an assignment to a struct member
//                         if (match("Constant")) {
//                             // Check if this is a floating-point literal
//                             if (checkText(".")) {
//                                 matchText("."); // Consume the decimal point
//                                 if (match("Constant")) {
//                                     // Successfully parsed a floating-point literal
//                                 }
//                             }
//                             // Simple constant assignment
//                             return true;
//                         } else if (match("String Literal") || match("Character Literal")) {
//                             // String or character literal assignment
//                             return true;
//                         } else if (parseExpression()) {
//                             return true;
//                         } else {
//                             error("Invalid expression in struct member assignment");
//                             return false;
//                         }
//                     } else {
//                         // It's a struct member access without assignment
//                         return true;
//                     }
//                 } else {
//                     error("Expected identifier after '->' operator");
//                     return false;
//                 }
//             }
//             // Handle array indexing
//             else if (matchText("[")) {
//                 // Parse array index
//                 if (match("Constant") || match("Identifier") || parseExpression()) {
//                     if (matchText("]")) {
//                         // Valid array access
//                         if (match("Assignment operator")) {
//                             // It's an assignment to an array element
//                             if (match("Constant") || match("String Literal") || match("Character Literal") || parseExpression()) {
//                                 return true;
//                             } else {
//                                 error("Invalid expression in array element assignment");
//                                 return false;
//                             }
//                         } else {
//                             // It's an array access without assignment
//                             return true;
//                         }
//                     } else {
//                         error("Expected ']' after array index");
//                         return false;
//                     }
//                 } else {
//                     error("Invalid array index expression");
//                     return false;
//                 }
//             }
//             // Handle function call
//             else if (matchText("(")) {
//                 // Parse argument list
//                 if (!checkText(")")) {
//                     do {
//                         if (checkText(")")) {
//                             break; // Empty argument list or end of arguments
//                         }
//
//                         if (!parseExpression()) {
//                             error("Invalid function argument");
//                             return false;
//                         }
//                     } while (matchText(","));
//                 }
//
//                 if (matchText(")")) {
//                     return true;
//                 } else {
//                     error("Expected ')' to close function call");
//                     return false;
//                 }
//             }
//             // Handle assignment
//             else if (match("Assignment operator")) {
//                 if (match("Constant") || match("String Literal") || match("Character Literal") || parseExpression()) {
//                     return true;
//                 } else {
//                     error("Invalid expression in assignment");
//                     return false;
//                 }
//             }
//             // Simple variable reference
//             else {
//                 return true;
//             }
//         }
//         // Check for constant
//         else if (match("Constant")) {
//             // Check if this is a floating-point literal
//             if (checkText(".")) {
//                 matchText("."); // Consume the decimal point
//                 if (match("Constant")) {
//                     // Successfully parsed a floating-point literal
//                 }
//             }
//             return true;
//         }
//         // Check for string literal
//         else if (match("String Literal")) {
//             return true;
//         }
//         // Check for character literal
//         else if (match("Character Literal")) {
//             return true;
//         }
//         // Check for parenthesized expression
//         else if (matchText("(")) {
//             if (parseExpression()) {
//                 if (matchText(")")) {
//                     return true;
//                 } else {
//                     error("Expected ')' to close expression");
//                     return false;
//                 }
//             } else {
//                 error("Invalid expression in parentheses");
//                 return false;
//             }
//         }
//
//         return false;
//     }
//
// public:
//     Parser(const vector<Token>& tokens) : tokens(tokens) {
//         // Initialize with some common types that might be used in the program
//         customTypes.insert("Person");
//     }
//
//     void parse() {
//         try {
//             parseProgram();
//         } catch (const std::exception& e) {
//             // Catch any exceptions and continue parsing
//             cerr << "Error during parsing: " << e.what() << endl;
//         }
//
//         // Print all errors found during parsing
//         cout << "Total NO of errors: " << errorCount << endl;
//     }
//
//     // Getter for error count
//     int getErrorCount() const {
//         return errorCount;
//     }
// };
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
//
// // Function to directly parse a string of code for testing
// vector<Token> createTestTokens() {
//     vector<Token> tokens;
//
//     // Error 1: Missing semicolon after variable declaration
//     tokens.push_back({1, "Integer", "Identifier"});
//     tokens.push_back({1, "x", "Identifier"});
//     tokens.push_back({1, "=", "Assignment operator"});
//     tokens.push_back({1, "5", "Constant"});
//     // Missing semicolon
//
//     // Error 2: Mismatched parentheses
//     tokens.push_back({3, "Integer", "Identifier"});
//     tokens.push_back({3, "test", "Identifier"});
//     tokens.push_back({3, "(", "Punctuator"});
//     tokens.push_back({3, "Integer", "Identifier"});
//     tokens.push_back({3, "a", "Identifier"});
//     tokens.push_back({3, ",", "Punctuator"});
//     tokens.push_back({3, "Integer", "Identifier"});
//     tokens.push_back({3, "b", "Identifier"});
//     tokens.push_back({3, "{", "Punctuator"});  // Should be ')'
//
//     // Error 3: Invalid struct member access
//     tokens.push_back({10, "Person", "Identifier"});
//     tokens.push_back({10, "p", "Identifier"});
//     tokens.push_back({10, ";", "Punctuator"});
//     tokens.push_back({11, "p", "Identifier"});
//     tokens.push_back({11, "->", "Access Operator"});
//     tokens.push_back({11, "namee", "Identifier"});  // Typo in member name
//     tokens.push_back({11, "=", "Assignment operator"});
//     tokens.push_back({11, "\"John\"", "String Literal"});
//     tokens.push_back({11, ";", "Punctuator"});
//
//     // Error 4: Missing closing brace
//     tokens.push_back({14, "if", "Keyword"});
//     tokens.push_back({14, "(", "Punctuator"});
//     tokens.push_back({14, "x", "Identifier"});
//     tokens.push_back({14, ">", "Operator"});
//     tokens.push_back({14, "10", "Constant"});
//     tokens.push_back({14, ")", "Punctuator"});
//     tokens.push_back({14, "{", "Punctuator"});
//     tokens.push_back({15, "x", "Identifier"});
//     tokens.push_back({15, "=", "Assignment operator"});
//     tokens.push_back({15, "x", "Identifier"});
//     tokens.push_back({15, "+", "Operator"});
//     tokens.push_back({15, "1", "Constant"});
//     tokens.push_back({15, ";", "Punctuator"});
//     // Missing closing brace
//
//     // Error 5: Invalid assignment
//     tokens.push_back({18, "10", "Constant"});
//     tokens.push_back({18, "=", "Assignment operator"});
//     tokens.push_back({18, "x", "Identifier"});
//     tokens.push_back({18, ";", "Punctuator"});
//
//     // Error 6: Undefined variable
//     tokens.push_back({21, "y", "Identifier"});
//     tokens.push_back({21, "=", "Assignment operator"});
//     tokens.push_back({21, "20", "Constant"});
//     tokens.push_back({21, ";", "Punctuator"});
//
//     // Error 7: Invalid syntax in function call
//     tokens.push_back({24, "test", "Identifier"});
//     tokens.push_back({24, "(", "Punctuator"});
//     tokens.push_back({24, "10", "Constant"});
//     tokens.push_back({24, ",", "Punctuator"});
//     tokens.push_back({24, ")", "Punctuator"});
//     tokens.push_back({24, ";", "Punctuator"});
//
//     // Error 8: Invalid floating point literal
//     tokens.push_back({27, "Float", "Identifier"});
//     tokens.push_back({27, "f", "Identifier"});
//     tokens.push_back({27, "=", "Assignment operator"});
//     tokens.push_back({27, "10", "Constant"});
//     tokens.push_back({27, ".", "Punctuator"});
//     tokens.push_back({27, ";", "Punctuator"});
//
//     // Error 9: Mismatched string literal
//     tokens.push_back({30, "String", "Identifier"});
//     tokens.push_back({30, "s", "Identifier"});
//     tokens.push_back({30, "=", "Assignment operator"});
//     tokens.push_back({30, "\"Hello world", "String Literal"});
//     tokens.push_back({30, ";", "Punctuator"});
//
//     // Error 10: Invalid character literal
//     tokens.push_back({33, "Character", "Identifier"});
//     tokens.push_back({33, "c", "Identifier"});
//     tokens.push_back({33, "=", "Assignment operator"});
//     tokens.push_back({33, "'ab'", "Character Literal"});
//     tokens.push_back({33, ";", "Punctuator"});
//
//     // Add EOF token to ensure proper termination
//     tokens.push_back({-1, "", "EOF"});
//
//     return tokens;
// }
//
// int main3() {
//     // Use our test tokens instead of loading from a file
//     vector<Token> tokens = createTestTokens();
//     Parser parser(tokens);
//     parser.parse();
//     return 0;
// }
