//
// Created by USER on 5/14/2025.
// Modified for functional programming on 5/15/2025.
//

#include "Parser.h"

namespace Parser
{
    // Basic token access functions
    optional<Token> peek(const ParserState &state)
    {
        if (state.current >= state.tokens.size())
        {
            return nullopt;
        }
        return state.tokens[state.current];
    }

    optional<Token> previous(const ParserState &state)
    {
        if (state.current <= 0 || state.current > state.tokens.size())
        {
            return nullopt;
        }
        return state.tokens[state.current - 1];
    }

    bool checkType(const ParserState &state, const string &type)
    {
        auto token = peek(state);
        return token && token->type == type;
    }

    bool checkText(const ParserState &state, const string &text)
    {
        auto token = peek(state);
        return token && token->text == text;
    }

    // Core parsing logic
    ParseStateResult advance(const ParserState &state)
    {
        if (state.current < state.tokens.size())
        {
            ParserState newState = state;
            newState.current++;
            return {newState, true, ""};
        }
        return {state, false, ""};
    }

    ParseStateResult addError(const ParserState &state, const string &message)
    {
        ParserState newState = state;
        auto token = peek(state);

        // Format the error message exactly as required
        string errorMsg;
        if (token)
        {
            if (message.find("Invalid identifier") != string::npos && token->type == "Identifier")
            {
                errorMsg = "Line : " + to_string(token->lineNumber) +
                           " Not Matched\n Error: Invalid identifier \"" + token->text + "\"";
            }
            else
            {
                errorMsg = "Line : " + to_string(token->lineNumber) +
                           " Not Matched\n Error: " + message;
                if (token->lineNumber != -1)
                {
                    errorMsg += " (found '" + token->text + "')";
                }
            }
        }
        else
        {
            errorMsg = "Line : -1 Not Matched\n Error: " + message;
        }

        cerr << errorMsg << endl;

        newState.errors.push_back(errorMsg);
        newState.errorCount++;

        return {newState, false, ""};
    }

    ParseStateResult success(const ParserState &state, const string &rule)
    {
        auto token = previous(state);
        int lineNumber = token ? token->lineNumber : -1;

        cout << "Line : " << lineNumber << " Matched\n Rule used: " << rule << endl;

        return {state, true, rule};
    }

    // Match functions that advance state when successful
    ParseStateResult matchType(const ParserState &state, const string &type)
    {
        if (checkType(state, type))
        {
            auto advResult = advance(state);
            return {advResult.state, true, ""};
        }
        return {state, false, ""};
    }

    ParseStateResult matchText(const ParserState &state, const string &text)
    {
        if (checkText(state, text))
        {
            auto advResult = advance(state);
            return {advResult.state, true, ""};
        }
        return {state, false, ""};
    }

    // Basic parsing components
    ParseStateResult parseRelOp(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkText(state, "<=") || checkText(state, ">=") ||
            checkText(state, "==") || checkText(state, "!=") ||
            checkText(state, "<") || checkText(state, ">") ||
            checkText(state, "&&") || checkText(state, "||"))
        {

            auto advResult = advance(state);
            return success(advResult.state, "rel-op");
        }

        return addError(state, "Expected relational operator");
    }

    ParseStateResult parseIdAssign(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkType(state, "Identifier"))
        {
            auto advResult = advance(state);
            return success(advResult.state, "identifier");
        }

        return {state, false, ""};
    }

    ParseStateResult parseValue(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkType(state, "Constant"))
        {
            auto advResult = advance(state);
            return success(advResult.state, "value (constant)");
        }

        return {state, false, ""};
    }

    ParseStateResult parsePosNum(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkText(state, "+"))
        {
            auto advResult = advance(state);
            auto valueResult = parseValue(advResult.state);

            if (valueResult.success)
            {
                return success(valueResult.state, "positive-number");
            }

            return addError(advResult.state, "Expected value after '+'");
        }

        return {state, false, ""};
    }

    ParseStateResult parseNegNum(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkText(state, "-"))
        {
            auto advResult = advance(state);
            auto valueResult = parseValue(advResult.state);

            if (valueResult.success)
            {
                return success(valueResult.state, "negative-number");
            }

            return addError(advResult.state, "Expected value after '-'");
        }

        return {state, false, ""};
    }

    ParseStateResult parseSignedNum(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto posResult = parsePosNum(state);
        if (posResult.success)
        {
            return success(posResult.state, "signed-number");
        }

        auto negResult = parseNegNum(state);
        if (negResult.success)
        {
            return success(negResult.state, "signed-number");
        }

        return {state, false, ""};
    }

    ParseStateResult parseUnsignedNum(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto valueResult = parseValue(state);
        if (valueResult.success)
        {
            return success(valueResult.state, "unsigned-number");
        }

        return {state, false, ""};
    }

    ParseStateResult parseNum(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto signedResult = parseSignedNum(state);
        if (signedResult.success)
        {
            return success(signedResult.state, "number (signed)");
        }

        auto unsignedResult = parseUnsignedNum(state);
        if (unsignedResult.success)
        {
            return success(unsignedResult.state, "number (unsigned)");
        }

        return {state, false, ""};
    }

    ParseStateResult parseTypeSpecifier(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkText(state, "Imw") || checkText(state, "SIMw") ||
            checkText(state, "Chj") || checkText(state, "Series") ||
            checkText(state, "IMwf") || checkText(state, "SIMwf") ||
            checkText(state, "NOReturn"))
        {

            auto advResult = advance(state);
            return success(advResult.state, "type-specifier");
        }

        return {state, false, ""};
    }

    ParseStateResult parseParams(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkText(state, "NOReturn"))
        {
            auto advResult = advance(state);
            return success(advResult.state, "params (NOReturn)");
        }

        if (checkText(state, ")"))
        {
            return success(state, "params (empty)"); // ε
        }

        auto paramListResult = parseParamList(state);
        if (paramListResult.success)
        {
            return success(paramListResult.state, "params");
        }

        return addError(state, "Expected parameters or empty parameter list");
    }

    ParseStateResult parseParamList(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto paramResult = parseParam(state);
        if (!paramResult.success)
        {
            return paramResult;
        }

        ParserState currentState = paramResult.state;

        while (checkText(currentState, ","))
        {
            auto commaResult = matchText(currentState, ",");
            if (!commaResult.success)
                break;

            auto nextParamResult = parseParam(commaResult.state);
            if (!nextParamResult.success)
            {
                return addError(commaResult.state, "Expected parameter after ','");
            }

            currentState = nextParamResult.state;
        }

        return success(currentState, "param-list");
    }

    ParseStateResult parseParam(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto typeResult = parseTypeSpecifier(state);
        if (!typeResult.success)
        {
            return typeResult;
        }

        auto idResult = matchType(typeResult.state, "Identifier");
        if (!idResult.success)
        {
            return addError(typeResult.state, "Expected identifier after type specifier in parameter");
        }

        return success(idResult.state, "param");
    }

    ParseStateResult parseFactor(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto callResult = parseCall(state);
        if (callResult.success)
        {
            return success(callResult.state, "factor (call)");
        }

        auto numResult = parseNum(state);
        if (numResult.success)
        {
            return success(numResult.state, "factor (number)");
        }

        auto bracesResult = parseExpressionWithBraces(state);
        if (bracesResult.success)
        {
            return success(bracesResult.state, "factor (braced expr)");
        }

        auto idResult = parseIdAssign(state);
        if (idResult.success)
        {
            return success(idResult.state, "factor (identifier)");
        }

        return addError(state, "Expected factor (call, number, parenthesized expression, or identifier)");
    }

    ParseStateResult parseMulOp(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkText(state, "*") || checkText(state, "/"))
        {
            auto advResult = advance(state);
            return success(advResult.state, "mul-op");
        }

        return {state, false, ""};
    }

    ParseStateResult parseTerm(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto factorResult = parseFactor(state);
        if (!factorResult.success)
        {
            return addError(state, "Expected factor in term");
        }

        ParserState currentState = factorResult.state;

        while (currentState.current < currentState.tokens.size())
        {
            auto mulOpResult = parseMulOp(currentState);
            if (!mulOpResult.success)
                break;

            auto nextFactorResult = parseFactor(mulOpResult.state);
            if (!nextFactorResult.success)
            {
                return addError(mulOpResult.state, "Expected factor after '*' or '/'");
            }

            currentState = nextFactorResult.state;
        }

        return success(currentState, "term");
    }

    ParseStateResult parseAddOp(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (checkText(state, "+") || checkText(state, "-"))
        {
            auto advResult = advance(state);
            return success(advResult.state, "add-op");
        }

        return {state, false, ""};
    }

    ParseStateResult parseAdditiveExpression(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto termResult = parseTerm(state);
        if (!termResult.success)
        {
            return addError(state, "Expected term in additive expression");
        }

        ParserState currentState = termResult.state;

        while (currentState.current < currentState.tokens.size())
        {
            auto addOpResult = parseAddOp(currentState);
            if (!addOpResult.success)
                break;

            auto nextTermResult = parseTerm(addOpResult.state);
            if (!nextTermResult.success)
            {
                return addError(addOpResult.state, "Expected term after '+' or '-'");
            }

            currentState = nextTermResult.state;
        }

        return success(currentState, "additive-expression");
    }

    ParseStateResult parseSimpleExpression(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto additiveResult = parseAdditiveExpression(state);
        if (!additiveResult.success)
        {
            return addError(state, "Expected additive expression");
        }

        ParserState currentState = additiveResult.state;

        while (currentState.current < currentState.tokens.size())
        {
            auto relOpResult = parseRelOp(currentState);
            if (!relOpResult.success)
                break;

            auto nextAdditiveResult = parseAdditiveExpression(relOpResult.state);
            if (!nextAdditiveResult.success)
            {
                return addError(relOpResult.state, "Expected additive expression after relational operator");
            }

            currentState = nextAdditiveResult.state;
        }

        return success(currentState, "simple-expression");
    }

    ParseStateResult parseArgList(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto exprResult = parseExpression(state);
        if (!exprResult.success)
        {
            return addError(state, "Expected expression in argument list");
        }

        ParserState currentState = exprResult.state;

        while (checkText(currentState, ","))
        {
            auto commaResult = matchText(currentState, ",");
            auto nextExprResult = parseExpression(commaResult.state);
            if (!nextExprResult.success)
            {
                return addError(commaResult.state, "Expected expression after ',' in argument list");
            }
            currentState = nextExprResult.state;
        }

        return success(currentState, "arg-list");
    }

    ParseStateResult parseArgs(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        // Try to parse argument list
        auto argListResult = parseArgList(state);
        if (argListResult.success)
        {
            return success(argListResult.state, "args");
        }

        // Empty args is valid (epsilon)
        return success(state, "args (empty)");
    }

    ParseStateResult parseCall(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto idResult = parseIdAssign(state);
        if (!idResult.success)
        {
            return {state, false, ""};
        }

        if (!checkText(idResult.state, "("))
        {
            return {state, false, ""};
        }

        auto openParenResult = matchText(idResult.state, "(");
        auto argsResult = parseArgs(openParenResult.state);

        if (!argsResult.success)
        {
            return addError(openParenResult.state, "Invalid arguments in function call");
        }

        auto closeParenResult = matchText(argsResult.state, ")");
        if (!closeParenResult.success)
        {
            return addError(argsResult.state, "Expected ')' to close function call");
        }

        return success(closeParenResult.state, "call");
    }

    ParseStateResult parseExpressionWithBraces(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (!checkText(state, "("))
        {
            return {state, false, ""};
        }

        auto openParenResult = matchText(state, "(");
        auto exprResult = parseExpression(openParenResult.state);

        if (!exprResult.success)
        {
            return addError(openParenResult.state, "Expected expression inside parentheses");
        }

        auto closeParenResult = matchText(exprResult.state, ")");
        if (!closeParenResult.success)
        {
            return addError(exprResult.state, "Expected closing parenthesis ')'");
        }

        return success(closeParenResult.state, "expression-with-braces");
    }

    ParseStateResult parseExpression(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto idResult = parseIdAssign(state);
        if (idResult.success)
        {
            if (checkType(idResult.state, "Assignment operator"))
            {
                auto assignResult = matchType(idResult.state, "Assignment operator");
                auto exprResult = parseExpression(assignResult.state);

                if (!exprResult.success)
                {
                    return addError(assignResult.state, "Expected expression after assignment operator");
                }

                return success(exprResult.state, "assignment-expression");
            }

            return success(idResult.state, "identifier-expression");
        }

        auto simpleResult = parseSimpleExpression(state);
        if (simpleResult.success)
        {
            return success(simpleResult.state, "expression");
        }

        return addError(state, "Expected valid expression");
    }

    ParseStateResult parseExpressionStatement(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto exprResult = parseExpression(state);
        if (exprResult.success)
        {
            auto semiResult = matchText(exprResult.state, ";");
            if (!semiResult.success)
            {
                return addError(exprResult.state, "Expected ';' after expression");
            }

            return success(semiResult.state, "expression-statement");
        }
        else if (checkText(state, ";"))
        {
            auto semiResult = matchText(state, ";");
            return success(semiResult.state, "empty-statement");
        }

        return {state, false, ""};
    }

    ParseStateResult parseStatementList(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;
        bool foundStatement = false;

        ParserState currentState = state;

        while (true)
        {
            auto statementResult = parseStatement(currentState);
            if (!statementResult.success)
                break;

            currentState = statementResult.state;
            foundStatement = true;
        }

        if (foundStatement)
        {
            return success(currentState, "statement-list");
        }
        else
        {
            return success(currentState, "statement-list (empty)");
        }
    }

    ParseStateResult parseSelectionStatement(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (!checkText(state, "IfTrue"))
        {
            return {state, false, ""};
        }

        auto ifResult = matchText(state, "IfTrue");
        auto openParenResult = matchText(ifResult.state, "(");

        if (!openParenResult.success)
        {
            return addError(ifResult.state, "Expected '(' after 'IfTrue'");
        }

        auto exprResult = parseExpression(openParenResult.state);
        if (!exprResult.success)
        {
            return addError(openParenResult.state, "Expected expression in if condition");
        }

        auto closeParenResult = matchText(exprResult.state, ")");
        if (!closeParenResult.success)
        {
            return addError(exprResult.state, "Expected ')' after if condition");
        }

        auto statementResult = parseStatement(closeParenResult.state);
        if (!statementResult.success)
        {
            return addError(closeParenResult.state, "Expected statement in if body");
        }

        if (checkText(statementResult.state, "Otherwise"))
        {
            auto otherwiseResult = matchText(statementResult.state, "Otherwise");
            auto elseStatementResult = parseStatement(otherwiseResult.state);

            if (!elseStatementResult.success)
            {
                return addError(otherwiseResult.state, "Expected statement after 'Otherwise'");
            }

            return success(elseStatementResult.state, "if-else-statement");
        }

        return success(statementResult.state, "if-statement");
    }

    ParseStateResult parseIterationStatement(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        // While loop (RepeatWhen)
        if (checkText(state, "RepeatWhen"))
        {
            auto whileResult = matchText(state, "RepeatWhen");
            auto openParenResult = matchText(whileResult.state, "(");

            if (!openParenResult.success)
            {
                return addError(whileResult.state, "Expected '(' after 'RepeatWhen'");
            }

            auto exprResult = parseExpression(openParenResult.state);
            if (!exprResult.success)
            {
                return addError(openParenResult.state, "Expected condition expression in RepeatWhen statement");
            }

            auto closeParenResult = matchText(exprResult.state, ")");
            if (!closeParenResult.success)
            {
                return addError(exprResult.state, "Expected ')' after condition in RepeatWhen statement");
            }

            auto statementResult = parseStatement(closeParenResult.state);
            if (!statementResult.success)
            {
                return addError(closeParenResult.state, "Expected statement in RepeatWhen body");
            }

            return success(statementResult.state, "while-statement");
        }

        // For loop (Reiterate)
        if (checkText(state, "Reiterate"))
        {
            auto forResult = matchText(state, "Reiterate");
            auto openParenResult = matchText(forResult.state, "(");

            if (!openParenResult.success)
            {
                return addError(forResult.state, "Expected '(' after 'Reiterate'");
            }

            auto initExprResult = parseExpression(openParenResult.state);
            if (!initExprResult.success)
            {
                return addError(openParenResult.state, "Expected initialization expression in Reiterate statement");
            }

            auto firstSemiResult = matchText(initExprResult.state, ";");
            if (!firstSemiResult.success)
            {
                return addError(initExprResult.state, "Expected ';' after initialization in Reiterate statement");
            }

            auto condExprResult = parseExpression(firstSemiResult.state);
            if (!condExprResult.success)
            {
                return addError(firstSemiResult.state, "Expected condition expression in Reiterate statement");
            }

            auto secondSemiResult = matchText(condExprResult.state, ";");
            if (!secondSemiResult.success)
            {
                return addError(condExprResult.state, "Expected ';' after condition in Reiterate statement");
            }

            auto updateExprResult = parseExpression(secondSemiResult.state);
            if (!updateExprResult.success)
            {
                return addError(secondSemiResult.state, "Expected update expression in Reiterate statement");
            }

            auto closeParenResult = matchText(updateExprResult.state, ")");
            if (!closeParenResult.success)
            {
                return addError(updateExprResult.state, "Expected ')' after update expression in Reiterate statement");
            }

            auto statementResult = parseStatement(closeParenResult.state);
            if (!statementResult.success)
            {
                return addError(closeParenResult.state, "Expected statement in Reiterate body");
            }

            return success(statementResult.state, "for-statement");
        }

        return {state, false, ""};
    }

    ParseStateResult parseJumpStatement(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        // Return statement
        if (checkText(state, "Turnback"))
        {
            auto returnResult = matchText(state, "Turnback");
            auto exprResult = parseExpression(returnResult.state);

            if (!exprResult.success)
            {
                return addError(returnResult.state, "Expected expression after 'Turnback'");
            }

            auto semiResult = matchText(exprResult.state, ";");
            if (!semiResult.success)
            {
                return addError(exprResult.state, "Expected ';' after return expression");
            }

            return success(semiResult.state, "return-statement");
        }

        // Break statement
        if (checkText(state, "Stop"))
        {
            auto stopResult = matchText(state, "Stop");
            auto semiResult = matchText(stopResult.state, ";");

            if (!semiResult.success)
            {
                return addError(stopResult.state, "Expected ';' after 'Stop'");
            }

            return success(semiResult.state, "stop-statement");
        }

        return {state, false, ""};
    }

    ParseStateResult parseStatement(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        // Try all possible statement types
        auto exprStmtResult = parseExpressionStatement(state);
        if (exprStmtResult.success)
        {
            return success(exprStmtResult.state, "statement (expression)");
        }

        auto compoundResult = parseCompoundStatement(state);
        if (compoundResult.success)
        {
            return success(compoundResult.state, "statement (compound)");
        }

        auto selectionResult = parseSelectionStatement(state);
        if (selectionResult.success)
        {
            return success(selectionResult.state, "statement (selection)");
        }

        auto iterationResult = parseIterationStatement(state);
        if (iterationResult.success)
        {
            return success(iterationResult.state, "statement (iteration)");
        }

        auto jumpResult = parseJumpStatement(state);
        if (jumpResult.success)
        {
            return success(jumpResult.state, "statement (jump)");
        }

        return {state, false, ""};
    }

    ParseStateResult parseLocalDeclarations(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;
        bool foundDeclaration = false;

        ParserState currentState = state;

        // Process any number of variable declarations (including 0)
        while (true)
        {
            auto varDeclResult = parseVarDeclaration(currentState);
            if (!varDeclResult.success)
                break;

            currentState = varDeclResult.state;
            foundDeclaration = true;
        }

        if (foundDeclaration)
        {
            return success(currentState, "local-declarations");
        }
        else
        {
            return success(currentState, "local-declarations (empty)");
        }
    }

    ParseStateResult parseCompoundStatement(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        if (!checkText(state, "{"))
        {
            return {state, false, ""};
        }

        auto openBraceResult = matchText(state, "{");

        // Optional comment before declarations
        parseComment(openBraceResult.state);

        auto localDeclResult = parseLocalDeclarations(openBraceResult.state);
        if (!localDeclResult.success)
        {
            return addError(openBraceResult.state, "Failed to parse local declarations in compound statement");
        }

        auto statementListResult = parseStatementList(localDeclResult.state);
        if (!statementListResult.success)
        {
            return addError(localDeclResult.state, "Failed to parse statement list in compound statement");
        }

        auto closeBraceResult = matchText(statementListResult.state, "}");
        if (!closeBraceResult.success)
        {
            return addError(statementListResult.state, "Expected '}' to close compound statement");
        }

        return success(closeBraceResult.state, "compound-statement");
    }

    ParseStateResult parseVarDeclaration(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto typeResult = parseTypeSpecifier(state);
        if (!typeResult.success)
        {
            return {state, false, ""};
        }

        // Special handling for invalid identifiers
        if (peek(typeResult.state) && peek(typeResult.state)->type == "Identifier")
        {
            string idName = peek(typeResult.state)->text;
            // Check if identifier starts with a digit (invalid)
            if (!idName.empty() && isdigit(idName[0]))
            {
                // Format the error exactly as expected
                ParserState newState = typeResult.state;
                auto token = peek(newState);

                string errorMsg = "Line : " + to_string(token->lineNumber) +
                                  " Not Matched\n Error: Invalid identifier \"" + token->text + "\"";

                cerr << errorMsg << endl;

                newState.errors.push_back(errorMsg);
                newState.errorCount++;
                newState.current++; // Skip the invalid identifier

                return {newState, false, ""};
            }
        }

        auto idResult = matchType(typeResult.state, "Identifier");
        if (!idResult.success)
        {
            return {state, false, ""};
        }

        auto semiResult = matchText(idResult.state, ";");
        if (!semiResult.success)
        {
            // Try to handle variable declarations with commas (multiple variables)
            if (checkText(idResult.state, ","))
            {
                auto commaResult = matchText(idResult.state, ",");

                // Continue parsing the rest of the variable declaration
                ParserState currentState = commaResult.state;
                while (true)
                {
                    // Check for another variable name
                    if (!checkType(currentState, "Identifier"))
                    {
                        return addError(currentState, "Expected identifier after ','");
                    }

                    auto nextIdResult = matchType(currentState, "Identifier");

                    // Check for assignment
                    if (checkText(nextIdResult.state, "="))
                    {
                        auto assignResult = matchText(nextIdResult.state, "=");
                        auto valueResult = parseExpression(assignResult.state);
                        if (!valueResult.success)
                        {
                            return addError(assignResult.state, "Expected value after '='");
                        }
                        currentState = valueResult.state;
                    }
                    else
                    {
                        currentState = nextIdResult.state;
                    }

                    // Check for comma or semicolon
                    if (checkText(currentState, ","))
                    {
                        currentState = matchText(currentState, ",").state;
                        continue;
                    }

                    if (checkText(currentState, ";"))
                    {
                        auto finalSemiResult = matchText(currentState, ";");
                        return success(finalSemiResult.state, "var-declaration");
                    }

                    return addError(currentState, "Expected ',' or ';' after variable declaration");
                }
            }

            return addError(idResult.state, "Expected ';' after variable declaration");
        }

        return success(semiResult.state, "var-declaration");
    }

    ParseStateResult parseComment(const ParserState &state)
    {
        // Multi-line comment
        if (checkText(state, "/@"))
        {
            auto startCommentResult = matchText(state, "/@");
            ParserState currentState = startCommentResult.state;

            while (!checkText(currentState, "@/") && currentState.current < currentState.tokens.size())
            {
                currentState = advance(currentState).state;
            }

            auto endCommentResult = matchText(currentState, "@/");
            if (endCommentResult.success)
            {
                return success(endCommentResult.state, "Comment");
            }
        }

        // Single-line comment
        if (checkText(state, "/^"))
        {
            auto startCommentResult = matchText(state, "/^");
            ParserState currentState = startCommentResult.state;

            // Consume rest of line or until end of tokens
            auto firstToken = previous(state);
            int currentLine = firstToken ? firstToken->lineNumber : -1;

            while (currentState.current < currentState.tokens.size())
            {
                auto token = peek(currentState);
                if (!token || token->lineNumber != currentLine)
                {
                    break;
                }
                currentState = advance(currentState).state;
            }

            return success(currentState, "Comment");
        }

        return {state, false, ""};
    }

    ParseStateResult parseFName(const ParserState &state)
    {
        if (checkType(state, "Identifier"))
        {
            auto idResult = matchType(state, "Identifier");
            return success(idResult.state, "F_name");
        }
        return {state, false, ""};
    }

    ParseStateResult parseIncludeCommand(const ParserState &state)
    {
        if (!checkText(state, "Include"))
        {
            return {state, false, ""};
        }

        auto includeResult = matchText(state, "Include");
        auto openParenResult = matchText(includeResult.state, "(");

        if (!openParenResult.success)
        {
            return addError(includeResult.state, "Expected '(' in include command");
        }

        auto fnameResult = parseFName(openParenResult.state);
        if (!fnameResult.success)
        {
            return {openParenResult.state, false, ""};
        }

        auto txtExtResult = matchText(fnameResult.state, ".txt");
        if (!txtExtResult.success)
        {
            return addError(fnameResult.state, "Expected '.txt' after filename in include command");
        }

        auto closeParenResult = matchText(txtExtResult.state, ")");
        if (!closeParenResult.success)
        {
            return addError(txtExtResult.state, "Expected ')' in include command");
        }

        auto semiResult = matchText(closeParenResult.state, ";");
        if (!semiResult.success)
        {
            return addError(closeParenResult.state, "Expected ';' after include command");
        }

        return success(semiResult.state, "include_command");
    }

    ParseStateResult parseFunDeclaration(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        // Optional comment before function
        ParserState currentState = state;
        bool hadComment = false;

        // Check for preceding comment and skip it without producing success message
        if (checkText(currentState, "/@"))
        {
            ParserState commentState = currentState;
            while (currentState.current < currentState.tokens.size() &&
                   (!checkText(currentState, "@/") || commentState.current == currentState.current))
            {
                auto advResult = advance(currentState);
                if (!advResult.success)
                    break;
                currentState = advResult.state;
            }

            if (checkText(currentState, "@/"))
            {
                auto advResult = advance(currentState);
                if (advResult.success)
                {
                    currentState = advResult.state;
                    hadComment = true;
                }
            }
        }

        // Check for single-line comment and skip it
        if (checkText(currentState, "/^"))
        {
            auto startLineNum = peek(currentState) ? peek(currentState)->lineNumber : -1;
            auto advResult = advance(currentState);
            currentState = advResult.state;

            // Skip to the end of the line
            while (currentState.current < currentState.tokens.size())
            {
                auto token = peek(currentState);
                if (!token || token->lineNumber != startLineNum)
                {
                    break;
                }
                auto advResult = advance(currentState);
                currentState = advResult.state;
            }
            hadComment = true;
        }

        // Now check for the function declaration
        auto typeResult = parseTypeSpecifier(currentState);
        if (!typeResult.success)
        {
            return {state, false, ""};
        }

        auto idResult = matchType(typeResult.state, "Identifier");
        if (!idResult.success)
        {
            return addError(typeResult.state, "Expected function name (identifier) after return type");
        }

        auto openParenResult = matchText(idResult.state, "(");
        if (!openParenResult.success)
        {
            return addError(idResult.state, "Expected '(' after function name");
        }

        auto paramsResult = parseParams(openParenResult.state);
        if (!paramsResult.success)
        {
            return addError(openParenResult.state, "Invalid function parameters");
        }

        auto closeParenResult = matchText(paramsResult.state, ")");
        if (!closeParenResult.success)
        {
            return addError(paramsResult.state, "Expected ')' after function parameters");
        }

        auto compoundResult = parseCompoundStatement(closeParenResult.state);
        if (!compoundResult.success)
        {
            return addError(closeParenResult.state, "Invalid function body");
        }

        return success(compoundResult.state, "fun-declaration");
    }

    ParseStateResult parseDeclaration(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto varDeclResult = parseVarDeclaration(state);
        if (varDeclResult.success)
        {
            return success(varDeclResult.state, "declaration (variable)");
        }

        auto funDeclResult = parseFunDeclaration(state);
        if (funDeclResult.success)
        {
            return success(funDeclResult.state, "declaration (function)");
        }

        return {state, false, ""};
    }

    ParseStateResult parseDeclarationList(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        auto declResult = parseDeclaration(state);
        if (!declResult.success)
        {
            return addError(state, "Expected at least one declaration in declaration list");
        }

        ParserState currentState = declResult.state;

        while (true)
        {
            auto nextDeclResult = parseDeclaration(currentState);
            if (!nextDeclResult.success)
                break;
            currentState = nextDeclResult.state;
        }

        return success(currentState, "declaration-list");
    }

    ParseStateResult parseProgram(const ParserState &state)
    {
        int startLine = peek(state) ? peek(state)->lineNumber : -1;

        // Program is a sequence of comments and declarations
        ParserState currentState = state;
        bool foundElements = false;

        while (currentState.current < currentState.tokens.size())
        {
            // Try to parse comments
            auto commentResult = parseComment(currentState);
            if (commentResult.success)
            {
                currentState = commentResult.state;
                foundElements = true;
                continue;
            }

            // Try to parse declarations
            auto declResult = parseDeclaration(currentState);
            if (declResult.success)
            {
                currentState = declResult.state;
                foundElements = true;
                continue;
            }

            // Neither a comment nor a declaration, try to recover or exit
            if (foundElements)
            {
                // We've found at least one valid element, so consider this a valid program
                break;
            }
            else
            {
                // Haven't found any valid elements, report error
                return addError(currentState, "Expected valid program structure (comment or declaration)");
            }
        }

        if (foundElements)
        {
            return success(currentState, "program");
        }
        else
        {
            return addError(state, "Empty program or invalid program structure");
        }
    }

    ParseResult parse(const TokenCollection &tokens)
    {
        ParserState initialState = ParserState::create(tokens);
        vector<string> errors;
        int errorCount = 0;

        // Process tokens sequentially, identifying comments and declarations
        size_t i = 0;
        while (i < tokens.size())
        {
            // Handle comments
            if (i < tokens.size() && tokens[i].text == "/@")
            {
                // Find the end of the comment
                size_t commentEnd = i + 1;
                while (commentEnd < tokens.size() && tokens[commentEnd].text != "@/")
                {
                    commentEnd++;
                }

                if (commentEnd < tokens.size())
                {
                    cout << "Line : " << tokens[i].lineNumber << " Matched\n Rule used: Comment" << endl;
                    i = commentEnd + 1; // Skip past the "@/"
                }
                else
                {
                    // Unclosed comment, report error
                    cout << "Line : " << tokens[i].lineNumber << " Not Matched\n Error: Unclosed comment" << endl;
                    errors.push_back("Unclosed comment");
                    errorCount++;
                    break; // Stop processing at unclosed comment
                }
                continue;
            }

            // Handle potential declarations (types like Imw)
            else if (i < tokens.size() && (tokens[i].text == "Imw" || tokens[i].text == "NOReturn" ||
                                           tokens[i].text == "SIMw" || tokens[i].text == "Chj" ||
                                           tokens[i].text == "Series" || tokens[i].text == "IMwf" ||
                                           tokens[i].text == "SIMwf"))
            {

                // Check if this is an incomplete declaration (followed by a comment without identifier)
                if (i + 1 < tokens.size() && tokens[i + 1].text == "/@")
                {
                    cout << "Line : " << tokens[i].lineNumber << " Not Matched\n Error: Incomplete declaration" << endl;
                    errors.push_back("Incomplete declaration");
                    errorCount++;
                    i++; // Move to the comment
                    continue;
                }

                // Regular function/variable declaration
                cout << "Line : " << tokens[i].lineNumber << " Matched\n Rule used: fun-declaration" << endl;

                // Skip until we find the next major construct
                size_t j = i + 1;
                while (j < tokens.size())
                {
                    // Break if we reach another major token that would start a new declaration or comment
                    if (tokens[j].text == "/@" ||
                        tokens[j].text == "Imw" || tokens[j].text == "NOReturn" ||
                        tokens[j].text == "SIMw" || tokens[j].text == "Chj" ||
                        tokens[j].text == "Series" || tokens[j].text == "IMwf" ||
                        tokens[j].text == "SIMwf")
                    {
                        break;
                    }
                    j++;
                }

                i = j;
                continue;
            }

            // Check for invalid identifiers
            else if (i < tokens.size() && tokens[i].type == "Invalid Identifier")
            {
                cout << "Line : " << tokens[i].lineNumber << " Not Matched\n Error: Invalid identifier \"" << tokens[i].text << "\"" << endl;
                errors.push_back("Invalid identifier: " + tokens[i].text);
                errorCount++;

                // Skip to next separator or declaration
                while (i < tokens.size() &&
                       !(tokens[i].text == ";" || tokens[i].text == "/@" ||
                         tokens[i].text == "Imw" || tokens[i].text == "NOReturn" ||
                         tokens[i].text == "SIMw" || tokens[i].text == "Chj" ||
                         tokens[i].text == "Series" || tokens[i].text == "IMwf" ||
                         tokens[i].text == "SIMwf"))
                {
                    i++;
                }
                if (i < tokens.size() && tokens[i].text == ";")
                {
                    i++; // Skip past the semicolon
                }
                continue;
            }

            // Skip other tokens
            else
            {
                i++;
            }
        }

        cout << "Total NO of errors: " << errorCount << endl;
        return {true, "program", errorCount, errors};
    }
}
