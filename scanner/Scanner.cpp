//
// Created by USER on 5/13/2025.
//

#include "Scanner.h"
void Scanner::initKeywords() {
    keywords = {
        {"IfTrue", "Condition"}, {"Otherwise", "Condition"},
        {"Imw", "Integer"}, {"SIMw", "SInteger"},
        {"Chj", "Character"}, {"Series", "String"},
        {"IMwf", "Float"}, {"SIMwf", "SFloat"},
        {"NOReturn", "Void"},
        {"RepeatWhen", "Loop"}, {"Reiterate", "Loop"},
        {"Turnback", "Return"}, {"OutLoop", "Break"},
        {"Loli", "Struct"}, {"Include", "Inclusion"},
    };
}
bool Scanner::isValidIdentifier(const string &token) {
    return regex_match(token, regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

bool Scanner::isNumber(const string &token) {
    return regex_match(token, regex("^-?[0-9]+(\\.[0-9]+)?$"));
}

void Scanner::addToken(int lineNumber, const string &text, const string &type) {
    tokens.push_back({lineNumber, text, type});
    if (type == "Invalid Identifier") {
        ++errorCount;
    }
}
// Process a token and determine its type
void Scanner::processToken(const string &token, int lineNumber) {
    if (keywords.count(token)) {
        addToken(lineNumber, token, keywords[token]);
    } else if (isValidIdentifier(token)) {
        addToken(lineNumber, token, "Identifier");
    } else if (isNumber(token)) {
        addToken(lineNumber, token, "Constant");
    } else {
        addToken(lineNumber, token, "Invalid Identifier");
    }
}
// Tokenize a line character by character for better handling of special characters

void Scanner::tokenizeLine(const string &line, int lineNumber) {
        string currentToken;
        bool inString = false;
        bool inChar = false;

        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];

            // Handle string literals
            if (c == '"') {
                if (inString) {
                    currentToken += c;
                    addToken(lineNumber, currentToken, "String Literal");
                    currentToken = "";
                    inString = false;
                } else {
                    if (!currentToken.empty()) {
                        processToken(currentToken, lineNumber);
                        currentToken = "";
                    }
                    currentToken += c;
                    inString = true;
                }
                continue;
            }

            // Handle character literals
            if (c == '\'') {
                if (inChar) {
                    currentToken += c;
                    addToken(lineNumber, currentToken, "Character Literal");
                    currentToken = "";
                    inChar = false;
                } else {
                    if (!currentToken.empty()) {
                        processToken(currentToken, lineNumber);
                        currentToken = "";
                    }
                    currentToken += c;
                    inChar = true;
                }
                continue;
            }

            // If we're in a string or char, just add the character
            if (inString || inChar) {
                currentToken += c;
                continue;
            }

            // Handle special characters and operators
            // if (c == ',') {
            //     if (!currentToken.empty()) {
            //         processToken(currentToken, lineNumber);
            //         currentToken = "";
            //     }
            //     addToken(lineNumber, string(1, c), "Quotation Mark");
            //     continue;
            // }
            if (c == ';' || c == ',' || c == ':' || c == '.') {
                if (!currentToken.empty()) {
                    processToken(currentToken, lineNumber);
                    currentToken = "";
                }
                addToken(lineNumber, string(1, c), "Punctuation");
                continue;
            }

            if (c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')') {
                if (!currentToken.empty()) {
                    processToken(currentToken, lineNumber);
                    currentToken = "";
                }
                addToken(lineNumber, string(1, c), "Braces");
                continue;
            }

            // Handle operators that could be part of multi-character operators
            if (c == '=' || c == '<' || c == '>' || c == '!' || c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '~') {
                // Special handling for negative numbers
                if (c == '-' && i + 1 < line.length() && isdigit(line[i + 1])) {
                    // This is likely a negative number
                    if (currentToken.empty() ||
                        (i > 0 && (line[i-1] == '(' || line[i-1] == '=' || line[i-1] == ',' ||
                                    line[i-1] == '+' || line[i-1] == '-' || line[i-1] == '*' || line[i-1] == '/'))) {
                        currentToken += c;
                        continue;
                    }
                }

                // Check for multi-character operators
                if (i + 1 < line.length()) {
                    char nextChar = line[i + 1];
                    if ((c == '=' && nextChar == '=') ||
                        (c == '!' && nextChar == '=') ||
                        (c == '<' && nextChar == '=') ||
                        (c == '>' && nextChar == '=') ||
                        (c == '&' && nextChar == '&') ||
                        (c == '|' && nextChar == '|') ||
                        (c == '-' && nextChar == '>')) {

                        if (!currentToken.empty()) {
                            processToken(currentToken, lineNumber);
                            currentToken = "";
                        }

                        string op = string(1, c) + string(1, nextChar);
                        if (op == "==") {
                            addToken(lineNumber, op, "Relational operator");
                        } else if (op == "!=" || op == "<=" || op == ">=") {
                            addToken(lineNumber, op, "Relational operator");
                        } else if (op == "&&" || op == "||") {
                            addToken(lineNumber, op, "Logic operators");
                        } else if (op == "->") {
                            addToken(lineNumber, op, "Access Operator");
                        }

                        i++; // Skip the next character
                        continue;
                    }
                }

                // Single character operators
                if (!currentToken.empty()) {
                    processToken(currentToken, lineNumber);
                    currentToken = "";
                }

                if (c == '=') {
                    addToken(lineNumber, string(1, c), "Assignment operator");
                } else if (c == '<' || c == '>' || c == '!') {
                    addToken(lineNumber, string(1, c), "Relational operator");
                } else if (c == '+' || c == '-' || c == '*' || c == '/') {
                    addToken(lineNumber, string(1, c), "Arithmetic Operation");
                } else if (c == '&' || c == '|' || c == '~') {
                    addToken(lineNumber, string(1, c), "Logic operators");
                }

                continue;
            }

            // Handle whitespace
            if (isspace(c)) {
                if (!currentToken.empty()) {
                    processToken(currentToken, lineNumber);
                    currentToken = "";
                }
                continue;
            }

            // Add to current token
            currentToken += c;
        }

        // Process any remaining token
        if (!currentToken.empty()) {
            if (inString) {
                addToken(lineNumber, currentToken, "Invalid String Literal");
            } else if (inChar) {
                addToken(lineNumber, currentToken, "Invalid Character Literal");
            } else {
                processToken(currentToken, lineNumber);
            }
        }
    }

void Scanner::handleInclude(const string& line, const string& currentFile, int lineNumber) {
        smatch match;
        if (regex_search(line, match, regex(R"(Include\s*\(\s*([^)\s]+)\s*\)\s*;)"))) {
            string includedFile = match[1];

            // Remove quotes if present
            if (includedFile.front() == '"' && includedFile.back() == '"') {
                includedFile = includedFile.substr(1, includedFile.length() - 2);
            }

            // Prevent recursive inclusion
            if (processedFiles.find(includedFile) != processedFiles.end()) {
                return;
            }
            processedFiles.insert(includedFile);

            // Add Include token
            addToken(lineNumber, "Include", "Inclusion");
            addToken(lineNumber, "(", "Braces");
            addToken(lineNumber, includedFile, "String Literal");
            addToken(lineNumber, ")", "Braces");
            addToken(lineNumber, ";", "Punctuation");

            // Process included file
            ifstream incFile(includedFile);
            if (!incFile.is_open()) {
                cerr << "Warning: Could not open included file " << includedFile << endl;
                return;
            }

            string incLine;
            int incLineNum = 1;
            while (getline(incFile, incLine)) {
                // Check if line is a comment, empty, or plain text before processing
                if (incLine.find("/^") != string::npos ||
                    incLine.find("/@") != string::npos ||
                    incLine.find("@/") != string::npos ||
                    incLine.empty() ||
                    regex_match(incLine, regex("^\\s*$")) ||
                    !regex_match(incLine, regex("^[\\s]*(Imw|SIMw|Chj|Series|IMwf|SIMwf|NOReturn|Loli|Include|IfTrue|Otherwise|RepeatWhen|Reiterate|Turnback|OutLoop).*"))) {
                    // If it's not a valid language construct, treat it as a comment
                    addToken(incLineNum, incLine, "Comment Content");
                } else {
                    scanLine(incLine, includedFile, incLineNum);
                }
                ++incLineNum;
            }
        }
    }

void Scanner::scanLine(string line, const string& filename, int lineNumber) {
        // Handle comments
        if (inMultiLineComment) {
            size_t endPos = line.find("@/");
            if (endPos != string::npos) {
                inMultiLineComment = false;
                addToken(lineNumber, "@/", "Comment End");
                //addToken(lineNumber, line.substr(0, endPos + 2), "Comment Content1");
                addToken(lineNumber, line.substr(0, endPos), "Comment Content");
                // Process rest of line after comment ends
                if (endPos + 2 < line.length()) {
                    scanLine(line.substr(endPos + 2), filename, lineNumber);
                }
            } else {
                // Still in multi-line comment
                addToken(lineNumber, line, "Comment Content");

            }
            return;
        }

        // Check for start of multi-line comment
        size_t startPos = line.find("/@");
        if (startPos != string::npos) {
            // Process part before comment
            if (startPos > 0) {
                tokenizeLine(line.substr(0, startPos), lineNumber);
            }

            inMultiLineComment = true;
            addToken(lineNumber, "/@", "Comment Start");
            //addToken(lineNumber, line.substr(startPos), "Comment Content");
            // Check if multi-line comment ends on same line
            size_t endPos = line.find("@/", startPos + 2);
            if (endPos != string::npos) {
                addToken(lineNumber, line.substr(startPos + 2, endPos), "Comment Content");
            }
            else {
                addToken(lineNumber, line.substr(startPos + 2), "Comment Content");
            }


            if (endPos != string::npos) {
                inMultiLineComment = false;
                 addToken(lineNumber, "@/", "Comment End");
                // Process rest of line after comment ends
                if (endPos + 2 < line.length()) {
                    scanLine(line.substr(endPos + 2), filename, lineNumber);
                }
            }
            return;
        }

        // Check for single-line comment
        size_t commentPos = line.find("/^");
        if (commentPos != string::npos) {
            // Process part before comment
            if (commentPos > 0) {
                tokenizeLine(line.substr(0, commentPos), lineNumber);
            }
            addToken(lineNumber, "/^", "Comment");
            //addToken(lineNumber, line.substr(commentPos), "Comment Content");
            addToken(lineNumber, line.substr(commentPos + 2), "Comment Content");
            return;
        }

        // Check for Include directive
        if (line.find("Include") != string::npos && line.find("(") != string::npos) {
            handleInclude(line, filename, lineNumber);
            return;
        }

        // Process normal line
        tokenizeLine(line, lineNumber);
    }

vector<Token> Scanner::scanFile(const string &filename) {
    processedFiles.clear();
    processedFiles.insert(filename);

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return tokens;
    }

    string line;
    int lineNumber = 1;
    while (getline(file, line)) {
        scanLine(line, filename, lineNumber);
        ++lineNumber;
    }

    // Check if multi-line comment was not closed
    if (inMultiLineComment) {
        cerr << "Warning: Unclosed multi-line comment at end of file" << endl;
        inMultiLineComment = false;
    }
    return tokens;
}

void Scanner::printTokens() const {
    for (const auto &token : tokens) {
        if (token.type == "Invalid Identifier" || token.type == "Invalid String Literal" || token.type == "Invalid Character Literal") {
            cout << "Line : " << token.lineNumber << " Error in Token Text: " << token.text << " Token Type: Invalid Identifier" << endl;
        } else {
            cout << "Line : " << token.lineNumber << " Token Text: " << token.text << " Token Type: " << token.type << endl;
        }
    }
    cout << "Total NO of errors: " << errorCount << endl;
}