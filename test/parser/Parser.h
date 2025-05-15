//
// Created by USER on 5/14/2025.
// Modified for functional programming on 5/15/2025.
//

#ifndef PARSER_H
#define PARSER_H
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>
#include <functional>
#include <optional>
#include "../common.h"

using namespace std;

namespace Parser
{
    // Parser state - immutable
    struct ParserState
    {
        TokenCollection tokens;
        size_t current;
        int errorCount;
        vector<string> errors;
        set<string> validTypes;
        set<string> customTypes;

        // Create initial parser state
        static ParserState create(const TokenCollection &tokens)
        {
            set<string> validTypes = {
                "Void", "Integer", "SInteger", "Character", "String", "Float", "SFloat"};

            set<string> customTypes = {"Person"};

            return {tokens, 0, 0, {}, validTypes, customTypes};
        }
    };

    // Parse result with an updated state and success flag
    struct ParseStateResult
    {
        ParserState state;
        bool success;
        string rule;
    };

    // Basic token access functions
    optional<Token> peek(const ParserState &state);
    optional<Token> previous(const ParserState &state);
    bool checkType(const ParserState &state, const string &type);
    bool checkText(const ParserState &state, const string &text);

    // Core parsing logic
    ParseStateResult advance(const ParserState &state);
    ParseStateResult addError(const ParserState &state, const string &message);
    ParseStateResult success(const ParserState &state, const string &rule);

    // Match functions
    ParseStateResult matchType(const ParserState &state, const string &type);
    ParseStateResult matchText(const ParserState &state, const string &text);

    // Basic parsing components
    ParseStateResult parseRelOp(const ParserState &state);
    ParseStateResult parseSimpleExpression(const ParserState &state);
    ParseStateResult parseIdAssign(const ParserState &state);
    ParseStateResult parseArgList(const ParserState &state);
    ParseStateResult parseArgs(const ParserState &state);
    ParseStateResult parseCall(const ParserState &state);
    ParseStateResult parseValue(const ParserState &state);
    ParseStateResult parsePosNum(const ParserState &state);
    ParseStateResult parseNegNum(const ParserState &state);
    ParseStateResult parseSignedNum(const ParserState &state);
    ParseStateResult parseUnsignedNum(const ParserState &state);
    ParseStateResult parseNum(const ParserState &state);
    ParseStateResult parseTypeSpecifier(const ParserState &state);
    ParseStateResult parseParams(const ParserState &state);
    ParseStateResult parseParamList(const ParserState &state);
    ParseStateResult parseParam(const ParserState &state);
    ParseStateResult parseFactor(const ParserState &state);
    ParseStateResult parseMulOp(const ParserState &state);
    ParseStateResult parseTerm(const ParserState &state);
    ParseStateResult parseAddOp(const ParserState &state);
    ParseStateResult parseAdditiveExpression(const ParserState &state);
    ParseStateResult parseExpression(const ParserState &state);
    ParseStateResult parseExpressionWithBraces(const ParserState &state);
    ParseStateResult parseExpressionStatement(const ParserState &state);
    ParseStateResult parseStatementList(const ParserState &state);
    ParseStateResult parseSelectionStatement(const ParserState &state);
    ParseStateResult parseIterationStatement(const ParserState &state);
    ParseStateResult parseJumpStatement(const ParserState &state);
    ParseStateResult parseStatement(const ParserState &state);
    ParseStateResult parseCompoundStatement(const ParserState &state);
    ParseStateResult parseVarDeclaration(const ParserState &state);
    ParseStateResult parseFunDeclaration(const ParserState &state);
    ParseStateResult parseLocalDeclarations(const ParserState &state);
    ParseStateResult parseDeclaration(const ParserState &state);
    ParseStateResult parseDeclarationList(const ParserState &state);
    ParseStateResult parseComment(const ParserState &state);
    ParseStateResult parseIncludeCommand(const ParserState &state);
    ParseStateResult parseFName(const ParserState &state);
    ParseStateResult parseProgram(const ParserState &state);

    // Main entry point for parsing
    ParseResult parse(const TokenCollection &tokens);
}

#endif // PARSER_H
