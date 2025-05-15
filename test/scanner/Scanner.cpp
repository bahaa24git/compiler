//
// Created by USER on 5/13/2025.
// Modified for functional programming on 5/15/2025.
//

#include "Scanner.h"

namespace Scanner
{
    bool isValidIdentifier(const string &token)
    {
        return regex_match(token, regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
    }

    bool isNumber(const string &token)
    {
        return regex_match(token, regex("^-?[0-9]+(\\.[0-9]+)?$"));
    }

    Token createToken(int lineNumber, const string &text, const string &type)
    {
        return {lineNumber, text, type};
    }

    TokenCollection processToken(const ScannerContext &context, int lineNumber, const string &token, TokenCollection tokens)
    {
        Token newToken;
        if (context.keywords.count(token))
        {
            newToken = createToken(lineNumber, token, context.keywords.at(token));
        }
        else if (isValidIdentifier(token))
        {
            newToken = createToken(lineNumber, token, "Identifier");
        }
        else if (isNumber(token))
        {
            newToken = createToken(lineNumber, token, "Constant");
        }
        else
        {
            newToken = createToken(lineNumber, token, "Invalid Identifier");
        }

        tokens.push_back(newToken);
        return tokens;
    }

    Result<ScanResult> tokenizeLine(const ScannerContext &context, const string &line, int lineNumber, TokenCollection tokens)
    {
        string currentToken;
        bool inString = false;
        bool inChar = false;
        int errorCount = 0;
        TokenCollection updatedTokens = tokens;
        ScannerContext updatedContext = context;

        for (size_t i = 0; i < line.length(); ++i)
        {
            char c = line[i];

            // Handle string literals
            if (c == '"')
            {
                if (inString)
                {
                    currentToken += c;
                    updatedTokens.push_back(createToken(lineNumber, currentToken, "String Literal"));
                    currentToken = "";
                    inString = false;
                }
                else
                {
                    if (!currentToken.empty())
                    {
                        updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                        if (updatedTokens.back().type == "Invalid Identifier")
                        {
                            errorCount++;
                        }
                        currentToken = "";
                    }
                    currentToken += c;
                    inString = true;
                }
                continue;
            }

            // Handle character literals
            if (c == '\'')
            {
                if (inChar)
                {
                    currentToken += c;
                    updatedTokens.push_back(createToken(lineNumber, currentToken, "Character Literal"));
                    currentToken = "";
                    inChar = false;
                }
                else
                {
                    if (!currentToken.empty())
                    {
                        updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                        if (updatedTokens.back().type == "Invalid Identifier")
                        {
                            errorCount++;
                        }
                        currentToken = "";
                    }
                    currentToken += c;
                    inChar = true;
                }
                continue;
            }

            // If we're in a string or char, just add the character
            if (inString || inChar)
            {
                currentToken += c;
                continue;
            }

            if (c == ',')
            {
                if (!currentToken.empty())
                {
                    updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                    if (updatedTokens.back().type == "Invalid Identifier")
                    {
                        errorCount++;
                    }
                    currentToken = "";
                }
                updatedTokens.push_back(createToken(lineNumber, string(1, c), "Quotation Mark"));
                continue;
            }

            if (c == ';')
            {
                if (!currentToken.empty())
                {
                    updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                    if (updatedTokens.back().type == "Invalid Identifier")
                    {
                        errorCount++;
                    }
                    currentToken = "";
                }
                updatedTokens.push_back(createToken(lineNumber, string(1, c), "End Of Statment"));
                continue;
            }

            if (c == ':' || c == '.')
            {
                if (!currentToken.empty())
                {
                    updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                    if (updatedTokens.back().type == "Invalid Identifier")
                    {
                        errorCount++;
                    }
                    currentToken = "";
                }
                updatedTokens.push_back(createToken(lineNumber, string(1, c), "Punctuation"));
                continue;
            }

            if (c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')')
            {
                if (!currentToken.empty())
                {
                    updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                    if (updatedTokens.back().type == "Invalid Identifier")
                    {
                        errorCount++;
                    }
                    currentToken = "";
                }
                updatedTokens.push_back(createToken(lineNumber, string(1, c), "Braces"));
                continue;
            }

            // Handle operators that could be part of multi-character operators
            if (c == '=' || c == '<' || c == '>' || c == '!' || c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '~')
            {
                // Special handling for negative numbers
                if (c == '-' && i + 1 < line.length() && isdigit(line[i + 1]))
                {
                    // This is likely a negative number
                    if (currentToken.empty() ||
                        (i > 0 && (line[i - 1] == '(' || line[i - 1] == '=' || line[i - 1] == ',' ||
                                   line[i - 1] == '+' || line[i - 1] == '-' || line[i - 1] == '*' || line[i - 1] == '/')))
                    {
                        currentToken += c;
                        continue;
                    }
                }

                // Check for multi-character operators
                if (i + 1 < line.length())
                {
                    char nextChar = line[i + 1];
                    if ((c == '=' && nextChar == '=') ||
                        (c == '!' && nextChar == '=') ||
                        (c == '<' && nextChar == '=') ||
                        (c == '>' && nextChar == '=') ||
                        (c == '&' && nextChar == '&') ||
                        (c == '|' && nextChar == '|') ||
                        (c == '-' && nextChar == '>'))
                    {

                        if (!currentToken.empty())
                        {
                            updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                            if (updatedTokens.back().type == "Invalid Identifier")
                            {
                                errorCount++;
                            }
                            currentToken = "";
                        }

                        string op = string(1, c) + string(1, nextChar);
                        if (op == "==")
                        {
                            updatedTokens.push_back(createToken(lineNumber, op, "Relational operator"));
                        }
                        else if (op == "!=" || op == "<=" || op == ">=")
                        {
                            updatedTokens.push_back(createToken(lineNumber, op, "Relational operator"));
                        }
                        else if (op == "&&" || op == "||")
                        {
                            updatedTokens.push_back(createToken(lineNumber, op, "Logic operators"));
                        }
                        else if (op == "->")
                        {
                            updatedTokens.push_back(createToken(lineNumber, op, "Access Operator"));
                        }

                        i++; // Skip the next character
                        continue;
                    }
                }

                // Single character operators
                if (!currentToken.empty())
                {
                    updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                    if (updatedTokens.back().type == "Invalid Identifier")
                    {
                        errorCount++;
                    }
                    currentToken = "";
                }

                if (c == '=')
                {
                    updatedTokens.push_back(createToken(lineNumber, string(1, c), "Assignment operator"));
                }
                else if (c == '<' || c == '>' || c == '!')
                {
                    updatedTokens.push_back(createToken(lineNumber, string(1, c), "Relational operator"));
                }
                else if (c == '+' || c == '-' || c == '*' || c == '/')
                {
                    updatedTokens.push_back(createToken(lineNumber, string(1, c), "Arithmetic Operation"));
                }
                else if (c == '&' || c == '|' || c == '~')
                {
                    updatedTokens.push_back(createToken(lineNumber, string(1, c), "Logic operators"));
                }

                continue;
            }

            // Handle whitespace
            if (isspace(c))
            {
                if (!currentToken.empty())
                {
                    updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                    if (updatedTokens.back().type == "Invalid Identifier")
                    {
                        errorCount++;
                    }
                    currentToken = "";
                }
                continue;
            }

            // Add to current token
            currentToken += c;
        }

        // Process any remaining token
        if (!currentToken.empty())
        {
            if (inString)
            {
                updatedTokens.push_back(createToken(lineNumber, currentToken, "Invalid String Literal"));
                errorCount++;
            }
            else if (inChar)
            {
                updatedTokens.push_back(createToken(lineNumber, currentToken, "Invalid Character Literal"));
                errorCount++;
            }
            else
            {
                updatedTokens = processToken(context, lineNumber, currentToken, updatedTokens);
                if (updatedTokens.back().type == "Invalid Identifier")
                {
                    errorCount++;
                }
            }
        }

        return Result<ScanResult>::ok({updatedTokens, errorCount, updatedContext});
    }

    Result<ScanResult> handleInclude(const ScannerContext &context, const string &line, const string &currentFile, int lineNumber, TokenCollection tokens)
    {
        smatch match;
        ScannerContext updatedContext = context;
        TokenCollection updatedTokens = tokens;
        int errorCount = 0;

        if (regex_search(line, match, regex(R"(Include\s*\(\s*([^)\s]+)\s*\)\s*;)")))
        {
            string includedFile = match[1];

            // Remove quotes if present
            if (includedFile.front() == '"' && includedFile.back() == '"')
            {
                includedFile = includedFile.substr(1, includedFile.length() - 2);
            }

            // Prevent recursive inclusion
            if (context.processedFiles.find(includedFile) != context.processedFiles.end())
            {
                return Result<ScanResult>::ok({tokens, 0, context});
            }

            // Create a new context with the included file added to processed files
            updatedContext.processedFiles.insert(includedFile);

            // Add Include token
            updatedTokens.push_back(createToken(lineNumber, "Include", "Inclusion"));
            updatedTokens.push_back(createToken(lineNumber, "(", "Braces"));
            updatedTokens.push_back(createToken(lineNumber, includedFile, "String Literal"));
            updatedTokens.push_back(createToken(lineNumber, ")", "Braces"));
            updatedTokens.push_back(createToken(lineNumber, ";", "Punctuation"));

            // Process included file
            ifstream incFile(includedFile);
            if (!incFile.is_open())
            {
                cerr << "Warning: Could not open included file " << includedFile << endl;
                return Result<ScanResult>::ok({updatedTokens, errorCount, updatedContext});
            }

            string incLine;
            int incLineNum = 1;
            while (getline(incFile, incLine))
            {
                // Check if line is a comment, empty, or plain text before processing
                if (incLine.find("/^") != string::npos ||
                    incLine.find("/@") != string::npos ||
                    incLine.find("@/") != string::npos ||
                    incLine.empty() ||
                    regex_match(incLine, regex("^\\s*$")) ||
                    !regex_match(incLine, regex("^[\\s]*(Imw|SIMw|Chj|Series|IMwf|SIMwf|NOReturn|Loli|Include|IfTrue|Otherwise|RepeatWhen|Reiterate|Turnback|OutLoop).*")))
                {
                    // If it's not a valid language construct, treat it as a comment
                    updatedTokens.push_back(createToken(incLineNum, incLine, "Comment Content"));
                }
                else
                {
                    // Process normal line in included file
                    auto result = scanLine(updatedContext, incLine, includedFile, incLineNum, updatedTokens);
                    if (result.success)
                    {
                        updatedTokens = result.value.tokens;
                        updatedContext = result.value.context;
                        errorCount += result.value.errorCount;
                    }
                }
                ++incLineNum;
            }
        }

        return Result<ScanResult>::ok({updatedTokens, errorCount, updatedContext});
    }

    Result<ScanResult> scanLine(const ScannerContext &context, const string &line, const string &filename, int lineNumber, TokenCollection tokens)
    {
        ScannerContext updatedContext = context;
        TokenCollection updatedTokens = tokens;
        int errorCount = 0;

        // Handle comments
        if (context.inMultiLineComment)
        {
            size_t endPos = line.find("@/");
            if (endPos != string::npos)
            {
                // Update context to indicate we're no longer in a multi-line comment
                updatedContext.inMultiLineComment = false;

                updatedTokens.push_back(createToken(lineNumber, "@/", "Comment End"));
                updatedTokens.push_back(createToken(lineNumber, line.substr(0, endPos), "Comment Content"));

                // Process rest of line after comment ends
                if (endPos + 2 < line.length())
                {
                    auto result = scanLine(updatedContext, line.substr(endPos + 2), filename, lineNumber, updatedTokens);
                    if (result.success)
                    {
                        updatedTokens = result.value.tokens;
                        updatedContext = result.value.context;
                        errorCount += result.value.errorCount;
                    }
                }
            }
            else
            {
                // Still in multi-line comment
                updatedTokens.push_back(createToken(lineNumber, line, "Comment Content"));
            }
            return Result<ScanResult>::ok({updatedTokens, errorCount, updatedContext});
        }

        // Check for start of multi-line comment
        size_t startPos = line.find("/@");
        if (startPos != string::npos)
        {
            // Process part before comment
            if (startPos > 0)
            {
                auto result = tokenizeLine(updatedContext, line.substr(0, startPos), lineNumber, updatedTokens);
                if (result.success)
                {
                    updatedTokens = result.value.tokens;
                    errorCount += result.value.errorCount;
                }
            }

            updatedContext.inMultiLineComment = true;
            updatedTokens.push_back(createToken(lineNumber, "/@", "Comment Start"));

            // Check if multi-line comment ends on same line
            size_t endPos = line.find("@/", startPos + 2);
            if (endPos != string::npos)
            {
                updatedTokens.push_back(createToken(lineNumber, line.substr(startPos + 2, endPos - (startPos + 2)), "Comment Content"));
            }
            else
            {
                updatedTokens.push_back(createToken(lineNumber, line.substr(startPos + 2), "Comment Content"));
            }

            if (endPos != string::npos)
            {
                updatedContext.inMultiLineComment = false;
                updatedTokens.push_back(createToken(lineNumber, "@/", "Comment End"));

                // Process rest of line after comment ends
                if (endPos + 2 < line.length())
                {
                    auto result = scanLine(updatedContext, line.substr(endPos + 2), filename, lineNumber, updatedTokens);
                    if (result.success)
                    {
                        updatedTokens = result.value.tokens;
                        updatedContext = result.value.context;
                        errorCount += result.value.errorCount;
                    }
                }
            }
            return Result<ScanResult>::ok({updatedTokens, errorCount, updatedContext});
        }

        // Check for single-line comment
        size_t commentPos = line.find("/^");
        if (commentPos != string::npos)
        {
            // Process part before comment
            if (commentPos > 0)
            {
                auto result = tokenizeLine(updatedContext, line.substr(0, commentPos), lineNumber, updatedTokens);
                if (result.success)
                {
                    updatedTokens = result.value.tokens;
                    errorCount += result.value.errorCount;
                }
            }
            updatedTokens.push_back(createToken(lineNumber, "/^", "Comment"));
            updatedTokens.push_back(createToken(lineNumber, line.substr(commentPos + 2), "Comment Content"));
            return Result<ScanResult>::ok({updatedTokens, errorCount, updatedContext});
        }

        // Check for Include directive
        if (line.find("Include") != string::npos && line.find("(") != string::npos)
        {
            auto result = handleInclude(updatedContext, line, filename, lineNumber, updatedTokens);
            if (result.success)
            {
                return result;
            }
        }

        // Process normal line
        auto result = tokenizeLine(updatedContext, line, lineNumber, updatedTokens);
        if (result.success)
        {
            updatedTokens = result.value.tokens;
            errorCount += result.value.errorCount;
        }

        return Result<ScanResult>::ok({updatedTokens, errorCount, updatedContext});
    }

    Result<ScanResult> scanFile(const string &filename)
    {
        ScannerContext context = ScannerContext::create();

        // Add current file to processed files
        context.processedFiles.insert(filename);

        TokenCollection tokens;
        int totalErrorCount = 0;

        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Error: Could not open file " << filename << endl;
            return Result<ScanResult>::err("Could not open file: " + filename);
        }

        string line;
        int lineNumber = 1;
        while (getline(file, line))
        {
            auto result = scanLine(context, line, filename, lineNumber, tokens);
            if (result.success)
            {
                tokens = result.value.tokens;
                context = result.value.context;
                totalErrorCount += result.value.errorCount;
            }
            else
            {
                return result;
            }
            ++lineNumber;
        }

        // Check if multi-line comment was not closed
        if (context.inMultiLineComment)
        {
            cerr << "Warning: Unclosed multi-line comment at end of file" << endl;
            // Return with unclosed comment warning but don't treat as error
            return Result<ScanResult>::ok({tokens, totalErrorCount, ScannerContext{context.keywords, false, context.processedFiles}});
        }

        return Result<ScanResult>::ok({tokens, totalErrorCount, context});
    }

    void printTokens(const TokenCollection &tokens)
    {
        int errorCount = 0;

        for (const auto &token : tokens)
        {
            if (token.type == "Invalid Identifier" || token.type == "Invalid String Literal" || token.type == "Invalid Character Literal")
            {
                cout << "Line : " << token.lineNumber << " Error in Token Text: " << token.text << " Token Type: Invalid Identifier" << endl;
                errorCount++;
            }
            else
            {
                cout << "Line : " << token.lineNumber << " Token Text: " << token.text << " Token Type: " << token.type << endl;
            }
        }

        cout << "Total NO of errors: " << errorCount << endl;
    }
}