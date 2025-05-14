//
// Created by USER on 5/14/2025.
//

#include "Parser.h"
Token Parser::peek()
{
    if (current >= tokens.size())
    {
        return {-1, "", "EOF"};
    }
    return tokens[current];
}

Token Parser::previous()
{
    return tokens[current - 1];
}

bool Parser::checkType(const string &type)
{
    if (current >= tokens.size())
        return false;
    return tokens[current].type == type;
}

bool Parser::checkText(const string &text)
{
    if (current >= tokens.size())
        return false;
    return tokens[current].text == text;
}

bool Parser::matchType(const string &type)
{
    if (checkType(type))
    {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchText(const string &text)
{
    if (checkText(text))
    {
        advance();
        return true;
    }
    return false;
}

void Parser::advance()
{
    if (current < tokens.size())
    {
        current++;
    }
}

void Parser::error(const string &message)
{
    Token token = peek();
    cerr << "Line : " << token.lineNumber << " Not Matched - Error: " << message;
    if (token.lineNumber != -1)
    {
        cerr << " (found '" << token.text << "')";
    }
    cerr << endl;
    errorCount++;
}

void Parser::success(const string &rule, int lineNumber)
{
    cout << "Line : " << lineNumber << " Matched Rule used: " << rule << endl;
}

///////////////////////////////14, 18-34 and 35. value → INT_NUM | FLOAT_NUM we only have constant
bool Parser::parseRelOp()
{
    int startLine = peek().lineNumber;
    if (matchText("<=") || matchText(">=") ||
        matchText("==") || matchText("!=") ||
        matchText("<") || matchText(">") ||
        matchText("&&") || matchText("||"))
    {
        success("rel-op", startLine);
        return true;
    }
    error("Expected relational operator");
    return false;
}

bool Parser::parseSimpleExpression()
{
    int startLine = peek().lineNumber;
    if (!parseAdditiveExpression())
    {
        error("Expected additive expression");
        return false;
    }

    while (current < tokens.size() && parseRelOp())
    {
        if (!parseAdditiveExpression())
        {
            error("Expected additive expression after relational operator");
            return false;
        }
    }

    success("simple-expression", startLine);
    return true;
}

bool Parser::parseIdAssign()
{
    int startLine = peek().lineNumber;
    if (matchType("Identifier"))
    {
        success("identifier", startLine);
        return true;
    }
    return false;
}

bool Parser::parseArgList()
{
    int startLine = peek().lineNumber;
    if (!parseExpression())
    {
        error("Expected expression in argument list");
        return false;
    }

    while (current < tokens.size() && matchText(","))
    {
        if (!parseExpression())
        {
            error("Expected expression after ',' in argument list");
            return false;
        }
    }

    success("arg-list", startLine);
    return true;
}

bool Parser::parseArgs()
{
    int startLine = peek().lineNumber;
    if (parseArgList())
    {
        success("args", startLine);
        return true;
    }
    // epsilon production (empty args)
    success("args (empty)", startLine);
    return true;
}

bool Parser::parseCall()
{
    int startLine = peek().lineNumber;
    if (parseIdAssign())
    {
        if (matchText("("))
        {
            if (parseArgs())
            {
                if (matchText(")"))
                {
                    success("call", startLine);
                    return true;
                }
                error("Expected ')' to close function call");
                return false;
            }
            error("Invalid arguments in function call");
            return false;
        }
        return false;
    }
    return false;
}

bool Parser::parseValue()
{
    int startLine = peek().lineNumber;
    if (matchType("Constant"))
    {
        success("value (constant)", startLine);
        return true;
    }
    return false;
}

bool Parser::parsePosNum()
{
    int startLine = peek().lineNumber;
    if (matchText("+"))
    {
        if (parseValue())
        {
            success("positive-number", startLine);
            return true;
        }
        error("Expected value after '+'");
        return false;
    }
    return false;
}

bool Parser::parseNegNum()
{
    int startLine = peek().lineNumber;
    if (matchText("-"))
    {
        if (parseValue())
        {
            success("negative-number", startLine);
            return true;
        }
        error("Expected value after '-'");
        return false;
    }
    return false;
}

bool Parser::parseSignedNum()
{
    int startLine = peek().lineNumber;
    if (parsePosNum())
    {
        success("signed-number", startLine);
        return true;
    }
    if (parseNegNum())
    {
        success("signed-number", startLine);
        return true;
    }
    return false;
}

bool Parser::parseUnsignedNum()
{
    int startLine = peek().lineNumber;
    if (parseValue())
    {
        success("unsigned-number", startLine);
        return true;
    }
    return false;
}

bool Parser::parseNum()
{
    int startLine = peek().lineNumber;
    if (parseSignedNum())
    {
        success("number (signed)", startLine);
        return true;
    }
    if (parseUnsignedNum())
    {
        success("number (unsigned)", startLine);
        return true;
    }
    return false;
}

bool Parser::parseTypeSpecifier()
{
    int startLine = peek().lineNumber;
    if (matchText("Imw") || matchText("SIMw") || matchText("Chj") ||
        matchText("Series") || matchText("IMwf") || matchText("SIMwf") ||
        matchText("NOReturn"))
    {
        success("type-specifier", startLine);
        return true;
    }
    return false;
}

bool Parser::parseParams()
{
    int startLine = peek().lineNumber;
    if (matchText("NOReturn"))
    {
        success("params (NOReturn)", startLine);
        return true;
    }

    if (checkText(")"))
    {
        success("params (empty)", startLine); // ε
        return true;
    }

    if (parseParamList())
    {
        success("params", startLine);
        return true;
    }

    error("Expected parameters or empty parameter list");
    return false;
}

bool Parser::parseParamList()
{
    int startLine = peek().lineNumber;

    if (parseParam())
    {
        while (matchText(","))
        {
            if (!parseParam())
            {
                error("Expected parameter after ','");
                return false;
            }
        }
        success("param-list", startLine);
        return true;
    }
    return false;
}

bool Parser::parseParam()
{
    int startLine = peek().lineNumber;
    if (parseTypeSpecifier())
    {
        if (matchType("Identifier"))
        {
            success("param", startLine);
            return true;
        }
        else
        {
            error("Expected identifier after type specifier in parameter");
        }
    }
    return false;
}

bool Parser::parseFactor()
{
    int startLine = peek().lineNumber;

    if (parseCall())
    {
        success("factor (call)", startLine);
        return true;
    }
    else if (parseNum())
    {
        success("factor (number)", startLine);
        return true;
    }
    else if (parseExpressionWithBraces())
    {
        success("factor (braced expr)", startLine);
        return true;
    }
    else if (parseIdAssign())
    {
        success("factor (identifier)", startLine);
        return true;
    }

    error("Expected factor (call, number, parenthesized expression, or identifier)");
    return false;
}
bool Parser::parseMulOp()
{
    int startLine = peek().lineNumber;
    if (matchText("*") || matchText("/"))
    {
        success("mul-op", startLine);
        return true;
    }
    return false;
}
bool Parser::parseTerm()
{
    int startLine = peek().lineNumber;
    if (!parseFactor())
    {
        error("Expected factor in term");
        return false;
    }

    while (current < tokens.size() && parseMulOp())
    {
        if (!parseFactor())
        {
            error("Expected factor after '*' or '/'");
            return false;
        }
    }

    success("term", startLine);
    return true;
}
bool Parser::parseAddOp()
{
    int startLine = peek().lineNumber;
    if (matchText("+") || matchText("-"))
    {
        success("add-op", startLine);
        return true;
    }
    return false;
}
bool Parser::parseAdditiveExpression()
{
    int startLine = peek().lineNumber;
    if (!parseTerm())
    {
        error("Expected term in additive expression");
        return false;
    }

    while (current < tokens.size() && parseAddOp())
    {
        if (!parseTerm())
        {
            error("Expected term after '+' or '-'");
            return false;
        }
    }

    success("additive-expression", startLine);
    return true;
}

bool Parser::parseExpression()
{
    int startLine = peek().lineNumber;
    if (parseIdAssign())
    {
        if (matchType("Assignment operator"))
        {
            if (parseExpression())
            {
                success("assignment-expression", startLine);
                return true;
            }
            error("Expected expression after assignment operator");
            return false;
        }
        success("identifier-expression", startLine);
        return true;
    }
    if (parseSimpleExpression())
    {
        success("expression", startLine);
        return true;
    }
    error("Expected valid expression");
    return false;
}
bool Parser::parseExpressionWithBraces()
{
    int startLine = peek().lineNumber;
    if (matchText("("))
    {
        if (parseExpression())
        {
            if (matchText(")"))
            {
                success("expression-with-braces", startLine);
                return true;
            }
            error("Expected closing parenthesis ')'");
            return false;
        }
        error("Expected expression inside parentheses");
        return false;
    }
    return false;
}
bool Parser::parseExpressionStatement()
{
    int startLine = peek().lineNumber;
    if (parseExpression())
    {
        if (matchText(";"))
        {
            success("expression-statement", startLine);
            return true;
        }
        error("Expected ';' after expression");
        return false;
    }
    else if (matchText(";"))
    {
        success("empty-statement", startLine);
        return true;
    }
    return false;
}
bool Parser::parseStatementList()
{
    int startLine = peek().lineNumber;
    bool foundStatement = false;

    while (parseStatement())
    {
        foundStatement = true;
    }

    if (foundStatement)
    {
        success("statement-list", startLine);
    }
    else
    {
        success("statement-list (empty)", startLine); // epsilon case
    }
    return true;
}
bool Parser::parseSelectionStatement()
{
    int startLine = peek().lineNumber;
    if (!matchText("IfTrue"))
    {
        return false;
    }

    if (!matchText("("))
    {
        error("Expected '(' after 'IfTrue'");
        return false;
    }

    if (!parseExpression())
    {
        error("Expected expression in if condition");
        return false;
    }

    if (!matchText(")"))
    {
        error("Expected ')' after if condition");
        return false;
    }

    if (!parseStatement())
    {
        error("Expected statement in if body");
        return false;
    }

    if (matchText("Otherwise"))
    {
        if (!parseStatement())
        {
            error("Expected statement after 'Otherwise'");
            return false;
        }
        success("if-else-statement", startLine);
    }
    else
    {
        success("if-statement", startLine);
    }

    return true;
}
bool Parser::parseIterationStatement()
{
    int startLine = peek().lineNumber;

    if (matchText("RepeatWhen"))
    {
        if (!matchText("("))
        {
            error("Expected '(' after 'RepeatWhen'");
            return false;
        }

        if (!parseExpression())
        {
            error("Expected condition expression in RepeatWhen statement");
            return false;
        }

        if (!matchText(")"))
        {
            error("Expected ')' after condition in RepeatWhen statement");
            return false;
        }

        if (!parseStatement())
        {
            error("Expected statement in RepeatWhen body");
            return false;
        }

        success("while-statement", startLine);
        return true;
    }

    if (matchText("Reiterate"))
    {
        if (!matchText("("))
        {
            error("Expected '(' after 'Reiterate'");
            return false;
        }

        if (!parseExpression())
        {
            error("Expected initialization expression in Reiterate statement");
            return false;
        }

        if (!matchText(";"))
        {
            error("Expected ';' after initialization in Reiterate statement");
            return false;
        }

        if (!parseExpression())
        {
            error("Expected condition expression in Reiterate statement");
            return false;
        }

        if (!matchText(";"))
        {
            error("Expected ';' after condition in Reiterate statement");
            return false;
        }

        if (!parseExpression())
        {
            error("Expected update expression in Reiterate statement");
            return false;
        }

        if (!matchText(")"))
        {
            error("Expected ')' after update expression in Reiterate statement");
            return false;
        }

        if (!parseStatement())
        {
            error("Expected statement in Reiterate body");
            return false;
        }

        success("for-statement", startLine);
        return true;
    }

    return false;
}
bool Parser::parseJumpStatement()
{
    int startLine = peek().lineNumber;

    if (matchText("Turnback"))
    {
        if (!parseExpression())
        {
            error("Expected expression after 'Turnback'");
            return false;
        }

        if (!matchText(";"))
        {
            error("Expected ';' after return expression");
            return false;
        }

        success("return-statement", startLine);
        return true;
    }

    if (matchText("Stop"))
    {
        if (!matchText(";"))
        {
            error("Expected ';' after 'Stop'");
            return false;
        }

        success("stop-statement", startLine);
        return true;
    }

    return false;
}
bool Parser::parseStatement()
{
    int startLine = peek().lineNumber;

    if (parseExpressionStatement())
    {
        success("statement (expression)", startLine);
        return true;
    }

    if (parseCompoundStatement())
    {
        success("statement (compound)", startLine);
        return true;
    }

    if (parseSelectionStatement())
    {
        success("statement (selection)", startLine);
        return true;
    }

    if (parseIterationStatement())
    {
        success("statement (iteration)", startLine);
        return true;
    }

    if (parseJumpStatement())
    {
        success("statement (jump)", startLine);
        return true;
    }

    return false;
}

bool Parser::parseCompoundStatement()
{
    int startLine = peek().lineNumber;

    if (!matchText("{"))
    {
        return false;
    }

    parseComment(); // optional comment before declarations

    if (!parseLocalDeclarations())
    {
        error("Failed to parse local declarations in compound statement");
        return false;
    }

    if (!parseStatementList())
    {
        error("Failed to parse statement list in compound statement");
        return false;
    }

    if (!matchText("}"))
    {
        error("Expected '}' to close compound statement");
        return false;
    }

    success("compound-statement", startLine);
    return true;
}
bool Parser::parseVarDeclaration()
{
    int startLine = peek().lineNumber;
    if (parseTypeSpecifier())
    {
        if (matchType("Identifier"))
        {
            if (matchText(";"))
            {
                success("var-declaration", startLine);
                return true;
            }
            else
            {
                error("Expected ';' after variable declaration");
            }
        }
    }
    return false;
}
bool Parser::parseFunDeclaration()
{
    int startLine = peek().lineNumber;

    if (parseComment())
    {
        if (parseTypeSpecifier() && matchType("Identifier"))
        {
            success("fun-declaration (comment-style)", previous().lineNumber);
            return true;
        }
        error("Expected type specifier and identifier after comment in function declaration");
        return false;
    }

    if (parseTypeSpecifier())
    {
        if (!matchType("Identifier"))
        {
            error("Expected function name (identifier) after return type");
            return false;
        }

        if (!matchText("("))
        {
            error("Expected '(' after function name");
            return false;
        }

        if (!parseParams())
        {
            error("Invalid function parameters");
            return false;
        }

        if (!matchText(")"))
        {
            error("Expected ')' after function parameters");
            return false;
        }

        if (!parseCompoundStatement())
        {
            error("Invalid function body");
            return false;
        }

        success("fun-declaration", startLine);
        return true;
    }

    return false;
}
bool Parser::parseLocalDeclarations()
{
    int startLine = peek().lineNumber;
    bool foundDeclaration = false;

    while (parseVarDeclaration())
    {
        foundDeclaration = true;
    }

    if (foundDeclaration)
    {
        success("local-declarations", startLine);
    }
    else
    {
        success("local-declarations (empty)", startLine); // epsilon case
    }
    return true;
}
bool Parser::parseDeclaration()
{
    int startLine = peek().lineNumber;

    if (parseVarDeclaration())
    {
        success("declaration (variable)", startLine);
        return true;
    }

    if (parseFunDeclaration())
    {
        success("declaration (function)", startLine);
        return true;
    }

    return false;
}
bool Parser::parseDeclarationList()
{
    int startLine = peek().lineNumber;

    if (!parseDeclaration())
    {
        error("Expected at least one declaration in declaration list");
        return false;
    }

    while (parseDeclaration())
        ;

    success("declaration-list", startLine);
    return true;
}

bool Parser::parseProgram()
{
    int startLine = peek().lineNumber;

    if (parseDeclarationList())
    {
        success("program (declaration-list)", startLine);
        return true;
    }

    if (parseComment())
    {
        success("program (comment)", startLine);
        return true;
    }

    if (parseIncludeCommand())
    {
        success("program (include)", startLine);
        return true;
    }

    error("Expected valid program structure");
    return false;
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



