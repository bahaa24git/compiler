#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>

using namespace std;

// Token structure from the original parser
struct Token {
    int lineNumber;
    string text;
    string type;
};

// A simplified version of the parser to test error handling
class ErrorTestParser {
private:
    vector<string> lines;
    int errorCount = 0;
    
    void reportError(int lineNumber, const string& message) {
        cout << "Error on line " << lineNumber << ": " << message << endl;
        errorCount++;
    }
    
    bool checkSemicolon(const string& line, int lineNumber) {
        // Check if the line should end with a semicolon but doesn't
        if (line.find("int") != string::npos || 
            line.find("float") != string::npos ||
            line.find("char") != string::npos ||
            line.find("string") != string::npos) {
            
            if (line.find("=") != string::npos && line.find(";") == string::npos) {
                reportError(lineNumber, "Missing semicolon after variable declaration");
                return false;
            }
        }
        return true;
    }
    
    bool checkParentheses(const string& line, int lineNumber) {
        // Count opening and closing parentheses
        int openCount = 0;
        int closeCount = 0;
        
        for (char c : line) {
            if (c == '(') openCount++;
            if (c == ')') closeCount++;
        }
        
        if (openCount != closeCount) {
            reportError(lineNumber, "Mismatched parentheses");
            return false;
        }
        return true;
    }
    
    bool checkBraces(const string& line, int lineNumber) {
        // Count opening and closing braces
        int openCount = 0;
        int closeCount = 0;
        
        for (char c : line) {
            if (c == '{') openCount++;
            if (c == '}') closeCount++;
        }
        
        if (openCount != closeCount) {
            reportError(lineNumber, "Mismatched braces");
            return false;
        }
        return true;
    }
    
    bool checkInvalidAssignment(const string& line, int lineNumber) {
        // Check for invalid assignments like "10 = x"
        regex pattern(R"(\b\d+\s*=)");
        if (regex_search(line, pattern)) {
            reportError(lineNumber, "Invalid assignment (cannot assign to a literal)");
            return false;
        }
        return true;
    }
    
    bool checkFloatingPoint(const string& line, int lineNumber) {
        // Check for invalid floating point literals like "10."
        regex pattern(R"(\b\d+\.\s*[;)])");
        if (regex_search(line, pattern)) {
            reportError(lineNumber, "Invalid floating point literal (missing digits after decimal point)");
            return false;
        }
        return true;
    }
    
    bool checkStringLiterals(const string& line, int lineNumber) {
        // Count opening and closing quotes
        int quoteCount = 0;
        for (char c : line) {
            if (c == '"') quoteCount++;
        }
        
        if (quoteCount % 2 != 0) {
            reportError(lineNumber, "Unclosed string literal");
            return false;
        }
        return true;
    }
    
    bool checkCharLiterals(const string& line, int lineNumber) {
        // Check for invalid character literals (more than one character)
        regex pattern(R"('..+')");
        if (regex_search(line, pattern)) {
            reportError(lineNumber, "Invalid character literal (too many characters)");
            return false;
        }
        return true;
    }
    
    bool checkUndefinedVariables(const string& line, int lineNumber, const set<string>& definedVars) {
        // This is a simplified check - in a real parser this would be more complex
        regex pattern(R"((\w+)\s*=)");
        smatch match;
        
        if (regex_search(line, match, pattern)) {
            string varName = match[1];
            if (definedVars.find(varName) == definedVars.end() && 
                varName != "int" && varName != "float" && 
                varName != "char" && varName != "string" &&
                !isdigit(varName[0])) {
                reportError(lineNumber, "Use of undefined variable '" + varName + "'");
                return false;
            }
        }
        return true;
    }
    
public:
    ErrorTestParser(const vector<string>& lines) : lines(lines) {}
    
    void parse() {
        set<string> definedVars;
        
        for (size_t i = 0; i < lines.size(); i++) {
            string line = lines[i];
            int lineNumber = i + 1;
            
            // Skip comments
            if (line.find("//") == 0 || line.find("/*") == 0 || line.empty()) {
                continue;
            }
            
            // Check for variable declarations to track defined variables
            regex declPattern(R"((int|float|char|string)\s+(\w+))");
            smatch declMatch;
            if (regex_search(line, declMatch, declPattern)) {
                definedVars.insert(declMatch[2]);
            }
            
            // Run various error checks
            checkSemicolon(line, lineNumber);
            checkParentheses(line, lineNumber);
            checkBraces(line, lineNumber);
            checkInvalidAssignment(line, lineNumber);
            checkFloatingPoint(line, lineNumber);
            checkStringLiterals(line, lineNumber);
            checkCharLiterals(line, lineNumber);
            checkUndefinedVariables(line, lineNumber, definedVars);
        }
        
        cout << "Total errors found: " << errorCount << endl;
    }
};

int main() {
    // Read the error test file
    ifstream file("error_test.txt");
    vector<string> lines;
    string line;
    
    while (getline(file, line)) {
        lines.push_back(line);
    }
    
    // Parse the file and check for errors
    ErrorTestParser parser(lines);
    parser.parse();
    
    return 0;
}
