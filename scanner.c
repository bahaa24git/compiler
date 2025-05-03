#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

Keyword keywords[] = {
    {"*", "Arithmetic Operation"},
    {"/", "Arithmetic Operation"},
    {"+", "Arithmetic Operation"},
    {"-", "Arithmetic Operation"},
    {"(", "Braces"},
    {")", "Braces"},
    {"{", "Braces"},
    {"}", "Braces"},
    {";", "End Of Statement"},
    {"==", "relational operator"},
    {">=", "relational operator"},
    {"<=", "relational operator"},
    {"!=", "relational operator"},
    {">", "relational operator"},
    {"<", "relational operator"},
    {"=", "Assignment operator"},
    {"IfTrue", "Condition"},
    {"Otherwise", "Condition"},
    {"Imw", "Integer"},
    {"SIMw", "SInteger"},
    {"Chj", "Character"},
    {"Series", "String"},
    {"IMwf", "Float"},
    {"SIMwf", "SFloat"},
    {"NOReturn", "Void"},
    {"RepeatWhen", "Loop"},
    {"Reiterate", "Loop"},
    {"Turnback", "Return"},
    {"OutLoop", "Break"},
    {"Loli", "Struct"},
    {"Include", "Inclusion"}
};

int keywordCount = sizeof(keywords) / sizeof(keywords[0]);

const char* getKeywordTokenType(const char* word) {
    for (int i = 0; i < keywordCount; i++) {
        if (strcmp(keywords[i].keyword, word) == 0) {
            return keywords[i].tokenType;
        }
    }
    return NULL;
}

int isValidIdentifier(const char *str) {
    if (!isalpha(str[0]) && str[0] != '_') return 0;
    for (int i = 1; str[i]; i++) {
        if (!isalnum(str[i]) && str[i] != '_') return 0;
    }
    return 1;
}
void tokenizeAndScanLine(char *line, int lineNumber, int *errorCount) {
    int len = strlen(line);
    for (int i = 0; i < len;) {
        // Skip whitespace
        if (isspace(line[i])) {
            i++;
            continue;
        }

        // Handle single-line comment "/^"
        if (line[i] == '/' && line[i + 1] == '^') {
            printf("Line: %d Token Text: /^ Token Type: Single Line Comment, Comment Content: %s\n",
                   lineNumber, line + i + 2);
            break;
        }

        // Handle multi-character operators or symbols
        if (line[i] == '/' && line[i + 1] == '@') {
            printf("Line: %d Token Text: /@ Token Type: Comment Start\n", lineNumber);
            i += 2;
            continue;
        }

        if (line[i] == '@' && line[i + 1] == '/') {
            printf("Line: %d Token Text: @/ Token Type: Comment End\n", lineNumber);
            i += 2;
            continue;
        }

        // Handle symbols like { } ( ) = ;
        if (strchr("{}()=;-", line[i])) {
            char temp[2];
            temp[0] = line[i];
            temp[1] = '\0';
            const char *symbol_type = getKeywordTokenType(temp);
            printf("Line: %d Token Text: %c Token Type: %s\n", lineNumber, line[i], symbol_type);
            i++;
            continue;
        }

        // Extract word/token
        int start = i;
        while (i < len && !isspace(line[i]) && !strchr("{}()=;-", line[i])) {
            if (line[i] == '/' && (line[i + 1] == '^' || line[i + 1] == '@')) break;
            i++;
        }
        char token[64];
        strncpy(token, &line[start], i - start);
        token[i - start] = '\0';

        const char *type = getKeywordTokenType(token);
        if (type) {
            printf("Line: %d Token Text: %s Token Type: %s\n", lineNumber, token, type);
        } else if (isValidIdentifier(token)) {
            printf("Line: %d Token Text: %s Token Type: Identifier\n", lineNumber, token);
        } else if (isdigit(token[0])) {
            printf("Line: %d Token Text: %s Token Type: Constant\n", lineNumber, token);
        } else {
            printf("Line: %d Error in Token Text: %s\n", lineNumber, token);
            (*errorCount)++;
        }
    }
}
void scanFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Cannot open file %s\n", filename);
        return;
    }

    char line[256];
    int lineNumber = 1;
    int errorCount = 0;
    int inMultilineComment = 0;

    while (fgets(line, sizeof(line), file)) {
        if (inMultilineComment) {
            if (strstr(line, "@/")) {
                inMultilineComment = 0;
                printf("Line: %d Token Text: @/ Token Type: Comment End\n", lineNumber);
            } else {
                printf("Line: %d Token Text: %s Token Type: Comment Content\n", lineNumber, strtok(line, "\n"));
            }
            lineNumber++;
            continue;
        }

        if (strstr(line, "/@")) {
            inMultilineComment = 1;
            printf("Line: %d Token Text: /@ Token Type: Comment Start\n", lineNumber);
            lineNumber++;
            continue;
        }

        tokenizeAndScanLine(line, lineNumber, &errorCount);
        lineNumber++;
    }


    printf("\nTotal NO of errors: %d\n", errorCount);
    fclose(file);
}
// void scanFile(const char *filename) {
//     FILE *file = fopen(filename, "r");
//     if (!file) {
//         printf("Cannot open file %s\n", filename);
//         return;
//     }
//
//     char line[256];
//     int lineNumber = 1;
//     int errorCount = 0;
//     int inMultilineComment = 0;
//
//     while (fgets(line, sizeof(line), file)) {
//         char *p = line;
//         char *line_for_comment_start;
//         if(strstr(line, "/^")) {
//             line_for_comment_start = malloc(strlen(line) + 1);
//             strcpy(line_for_comment_start,p);
//         }
//
//         // Handle multiline comment start
//         if (strstr(p, "/@")) {
//             inMultilineComment = 1;
//             printf("Line: %d Token Text: /@ Token Type: Comment Start\n", lineNumber);
//             continue;
//         }
//
//         // Handle multiline comment end
//         if (inMultilineComment) {
//             if (strstr(p, "@/")) {
//                 inMultilineComment = 0;
//                 printf("Line: %d Token Text: @/ Token Type: Comment End\n", lineNumber);
//             } else {
//                 printf("Line: %d Token Text: %s Token Type: Comment Content\n", lineNumber, strtok(line, "\n"));
//             }
//             lineNumber++;
//             continue;
//         }
//
//         char *token = strtok(line, " \t\n;");
//         while (token != NULL) {
//
//             if (strstr(token, "/^")) {
//                 char *comment_start = strstr(line_for_comment_start, "/^");
//                 comment_start += 2;
//                 printf("Line: %d Token Text: /^ Token Type: Single Line Comment, Comment Content: %s\n", lineNumber,comment_start);
//                 free(line_for_comment_start);
//                 if(!(token[0] == '/' && token[1] == '^')) {
//                     token = strtok(token, "/^");
//                 }
//                 else {
//                     break;
//                 }
//             }
//             const char *type = getKeywordTokenType(token);
//
//             if (type) {
//                 printf("Line: %d Token Text: %s Token Type: %s\n", lineNumber, token, type);
//             } else if (isValidIdentifier(token)) {
//                 printf("Line: %d Token Text: %s Token Type: Identifier\n", lineNumber, token);
//             } else if (isdigit(token[0])) {
//                 printf("Line: %d Token Text: %s Token Type: Constant\n", lineNumber, token);
//             } else if (strcmp(token, "=") == 0) {
//                 printf("Line: %d Token Text: = Token Type: Assignment operator\n", lineNumber);
//             }
//             else {
//                 printf("Line: %d Error in Token Text: %s\n", lineNumber, token);
//                 errorCount++;
//             }
//
//             token = strtok(NULL, " \t\n;");
//         }
//         lineNumber++;
//     }
//
//     printf("\nTotal NO of errors: %d\n", errorCount);
//     fclose(file);
// }

