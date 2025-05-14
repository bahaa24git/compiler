// // Required standard library includes
// #include <iostream>
// #include <fstream>
// #include <stack>
// #include <stdio.h>   // For input/output operations
// #include <stdlib.h>  // For general utilities
// #include <string.h>  // For string manipulation functions
// #include <ctype.h>   // For character type checks
// #include "scanner.h" // Local header file containing Keyword struct definition
//
// using namespace std;
// // Array of keywords and their corresponding token types
// // This array defines the language's keywords and operators
// Keyword keywords[] = {
//     {",", "Comma Separator"},      // Comma token
//     {"*", "Arithmetic Operation"}, // Multiplication operator
//     {"/", "Arithmetic Operation"}, // Division operator
//     {"+", "Arithmetic Operation"}, // Addition operator
//     {"-", "Arithmetic Operation"}, // Subtraction operator
//     {"(", "Braces"},               // Opening parenthesis
//     {")", "Braces"},               // Closing parenthesis
//     {"{", "Braces"},               // Opening curly brace
//     {"}", "Braces"},               // Closing curly brace
//     {";", "End Of Statement"},     // Statement terminator
//     {"==", "Relational Operator"}, // Equality comparison
//     {">=", "Relational Operator"}, // Greater than or equal
//     {"<=", "Relational Operator"}, // Less than or equal
//     {"!=", "Relational Operator"}, // Not equal
//     {">", "Relational Operator"},  // Greater than
//     {"<", "Relational Operator"},  // Less than
//     {"=", "Assignment Operator"},  // Assignment operator
//     {"IfTrue", "Condition"},       // If statement
//     {"Otherwise", "Condition"},    // Else statement
//     {"Imw", "Integer"},            // Integer type
//     {"SIMw", "SInteger"},          // Signed Integer type
//     {"Chj", "Character"},          // Character type
//     {"Series", "String"},          // String type
//     {"IMwf", "Float"},             // Float type
//     {"SIMwf", "SFloat"},           // Signed Float type
//     {"NOReturn", "Void"},          // Void return type
//     {"RepeatWhen", "Loop"},        // While loop
//     {"Reiterate", "Loop"},         // For loop
//     {"Turnback", "Return"},        // Return statement
//     {"OutLoop", "Break"},          // Break statement
//     {"Loli", "Struct"},            // Structure definition
//     {"Include", "Inclusion"}       // Include directive
// };
//
// // Calculate the number of keywords in the array
// int keywordCount = sizeof(keywords) / sizeof(keywords[0]);
//
// // Function to get the token type for a given keyword
// const char *getKeywordTokenType(const char *word)
// {
//     // Loop through all keywords to find a match
//     for (int i = 0; i < keywordCount; i++)
//     {
//         if (strcmp(keywords[i].keyword, word) == 0)
//         {
//             return keywords[i].tokenType;
//         }
//     }
//     return NULL; // Return NULL if keyword not found
// }
//
// // Function to check if a string is a valid identifier
// int isValidIdentifier(const char *str)
// {
//     // First character must be a letter or underscore
//     if (!isalpha(str[0]) && str[0] != '_')
//         return 0;
//     // Subsequent characters must be alphanumeric or underscore
//     for (int i = 1; str[i]; i++)
//     {
//         if (!isalnum(str[i]) && str[i] != '_')
//             return 0;
//     }
//     return 1;
// }
//
// // Function to check if a string contains only digits
// int isdigit_custom(const char *token)
// {
//     // First character must be a digit
//     if (!isdigit(token[0]))
//         return 0;
//     // Check remaining characters
//     for (int i = 1; token[i]; i++)
//     {
//         if (!isdigit(token[i]))
//             return 0;
//     }
//     return 1;
// }
//
// // Function to tokenize and scan a single line of code
// void tokenizeAndScanLine(char *line, int lineNumber, int *errorCount)
// {
//     int len = strlen(line);
//     // Process each character in the line
//     for (int i = 0; i < len;)
//     {
//         // Skip whitespace characters
//         if (isspace(line[i]))
//         {
//             i++;
//             continue;
//         }
//
//         // Handle single-line comment "/^"
//         if (line[i] == '/' && line[i + 1] == '^')
//         {
//             printf("Line: %d Token Text: /^ Token Type: Single Line Comment, Comment Content: %s\n",
//                    lineNumber, line + i + 2);
//             break; // Rest of line is comment, stop processing
//         }
//         // Handle symbols like { } ( ) = ;
//         if (strchr("{}()=;,+-/*><", line[i]))
//         {
//             char temp[2];
//             temp[0] = line[i];
//             temp[1] = '\0';
//             const char *symbol_type = getKeywordTokenType(temp);
//             printf("Line: %d Token Text: %c Token Type: %s\n", lineNumber, line[i], symbol_type);
//             i++;
//             continue;
//         }
//
//         // Extract multi-character token
//         int start = i;
//         while (i < len && !isspace(line[i]) && !strchr("{}()=;,+-/*><", line[i]))
//         {
//             if (line[i] == '/' && (line[i + 1] == '^' || line[i + 1] == '@'))
//                 break;
//             i++;
//         }
//
//         // Create token string with bounds checking
//         char token[64];
//         size_t tokenLen = i - start;
//         if (tokenLen >= sizeof(token))
//         {
//             printf("Line: %d Error: Token too long\n", lineNumber);
//             (*errorCount)++;
//             i = start + 1; // Skip one char to avoid infinite loop
//             continue;
//         }
//         strncpy(token, &line[start], tokenLen);
//         token[tokenLen] = '\0';
//
//         // Classify and print token
//         const char *type = getKeywordTokenType(token);
//         if (type)
//         {
//             // Token is a keyword
//             printf("Line: %d Token Text: %s Token Type: %s\n", lineNumber, token, type);
//         }
//         else if (isValidIdentifier(token))
//         {
//             // Token is an identifier
//             printf("Line: %d Token Text: %s Token Type: Identifier\n", lineNumber, token);
//         }
//         else if (isdigit_custom(token))
//         {
//             // Token is a numeric constant
//             printf("Line: %d Token Text: %s Token Type: Constant\n", lineNumber, token);
//         }
//         else
//         {
//             // Invalid token
//             printf("Line: %d Error in Token Text: %s\n", lineNumber, token);
//             (*errorCount)++;
//         }
//     }
// }
//
// // Main function to scan and tokenize a source file
// void scanFile(const char *filename)
// {
//     // Open file for reading
//     // FILE *file = fopen(filename, "r");
//     // if (!file) {
//     //     printf("Cannot open file %s\n", filename);
//     //     return;
//     // }
//
//     // Initialize scanning variables
//     char line[256]; // Buffer for reading lines
//     string lineContent;
//     int lineNumber = 1;            // Current line number
//     int errorCount = 0;            // Total error count
//     int inMultilineComment = 0;    // Flag for multiline comment state
//     char commentBuffer[1000] = ""; // Buffer for collecting multiline comments
//
//     // Process file line by line
//     // while (fgets(line, sizeof(line), file)) {
//     //     // Look for comment markers
//     //     char *commentStart = strstr(line, "/@");
//     //     char *commentEnd = strstr(line, "@/");
//
//     //     // Handle multiline comment continuation
//     //     if (inMultilineComment) {
//     //         if (commentEnd) {
//     //             // End of multiline comment found
//     //             inMultilineComment = 0;
//     //             *commentEnd = '\0';
//     //             strcat(commentBuffer, line);  // Add final line to comment
//     //             printf("[Comment Block] %s\n", commentBuffer);
//     //             printf("Line: %d Token Text: @/ Token Type: Comment End\n", lineNumber);
//     //             strcpy(commentBuffer, "");  // Reset comment buffer
//
//     //             // Process any code after the comment ends
//     //             char *after = commentEnd + 2;
//     //             if (*after && strlen(after) > 0)
//     //                 tokenizeAndScanLine(after, lineNumber, &errorCount);
//     //         } else {
//     //             // Continue collecting multiline comment
//     //             strcat(commentBuffer, line);
//     //         }
//     //         lineNumber++;
//     //         continue;
//     //     }
//
//     //     // Handle single-line multiline comment
//     //     if (commentStart && commentEnd && commentStart < commentEnd) {
//     //         *commentStart = '\0';
//     //         char *after = commentEnd + 2;
//     //         tokenizeAndScanLine(line, lineNumber, &errorCount);
//     //         printf("Line: %d Token Text: /@ Token Type: Comment Start\n", lineNumber);
//     //         printf("[Comment Block] %.*s\n", (int)(commentEnd - commentStart - 2), commentStart + 2);
//     //         printf("Line: %d Token Text: @/ Token Type: Comment End\n", lineNumber);
//     //         if (*after && strlen(after) > 0)
//     //             tokenizeAndScanLine(after, lineNumber, &errorCount);
//     //     }
//     //     // Handle start of multiline comment
//     //     else if (commentStart) {
//     //         *commentStart = '\0';
//     //         tokenizeAndScanLine(line, lineNumber, &errorCount);
//     //         printf("Line: %d Token Text: /@ Token Type: Comment Start\n", lineNumber);
//     //         strcat(commentBuffer, commentStart + 2);
//     //         inMultilineComment = 1;
//     //     }
//     //     // Handle normal code line
//     //     else {
//     //         tokenizeAndScanLine(line, lineNumber, &errorCount);
//     //     }
//
//     //     lineNumber++;
//     // }
//
//     // // Check for unclosed multiline comment
//     // if (inMultilineComment) {
//     //     printf("Error: Unclosed multiline comment at end of file\n");
//     //     errorCount++;
//     // }
//
//     // // Print final error count and close file
//     // printf("\nTotal NO of errors: %d\n", errorCount);
//     // // Close the file after processing
//     // fclose(file);
//
//     ifstream MyReadFile(filename);
//
//     if (!MyReadFile.is_open())
//     {
//         cout << "Error opening file: " << filename << endl;
//         return;
//     }
//
//     char ch;
//     int columnNumber = 1;
//
//     while (MyReadFile.get(ch))
//     {
//
//         cout << "Line: " << lineNumber << " Column: " << columnNumber << " Character: " ;
//
//         // Print ASCII value for special characters
//         // if (ch < 33 || ch > 127)
//         // {
//         //     cout << " (ASCII: " << (int)ch << ")";
//         // }
//         if (ch == '\n')
//         {
//             cout << " (newline)";
//         }
//         else if (ch == '\t')
//         {
//             cout << " (tab)";
//         }
//         else if (ch == ' ')
//         {
//             cout << " (space)";
//         }
//         else
//         {
//             cout << ch;
//         }
//
//
//         // Track line and column numbers
//         if (ch == '\n')
//         {
//             lineNumber++;
//             columnNumber = 1;
//         }
//         else
//         {
//             columnNumber++;
//         }
//         cout << endl;
//     }
//
//     // Close the file
//     // MyReadFile.close();
// }
