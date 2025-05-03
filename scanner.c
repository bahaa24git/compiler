#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

Keyword keywords[] = {
    {",", "Commma Seperator"},
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
int isdigit_custom(const char *token) {
    if (!isdigit(token[0])) return 0;
    for (int i = 1; token[i]; i++) {
        if (!isdigit(token[i])) return 0;
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
        // Handle symbols like { } ( ) = ;
        if (strchr("{}()=;,+-/*><", line[i])) {
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
        while (i < len && !isspace(line[i]) && !strchr("{}()=;,+-/*><", line[i])) {
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
        } else if (isdigit_custom(token)) {
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
    char commentBuffer[1000] = "";  // To hold comment text
    while (fgets(line, sizeof(line), file)) {
        char *commentStart = strstr(line, "/@");
        char *commentEnd = strstr(line, "@/");
        if (inMultilineComment) {
            if (commentEnd) {
                inMultilineComment = 0;
                *commentEnd = '\0';
                strcat(commentBuffer, line);  // Add current line up to @/
                printf("[Comment Block] %s\n", commentBuffer);
                printf("Line: %d Token Text: @/ Token Type: Comment End\n", lineNumber);
                strcpy(commentBuffer, "");  // Reset
                // Process anything after the comment
                char *after = commentEnd + 2;
                if (*after && strlen(after) > 0)
                    tokenizeAndScanLine(after, lineNumber, &errorCount);
            } else {
                strcat(commentBuffer, line);  // Keep accumulating comment lines
            }
            lineNumber++;
            continue;
        }
        if (commentStart && commentEnd && commentStart < commentEnd) {
            // Same line comment
            *commentStart = '\0';
            char *after = commentEnd + 2;
            tokenizeAndScanLine(line, lineNumber, &errorCount);
            printf("Line: %d Token Text: /@ Token Type: Comment Start\n", lineNumber);
            printf("[Comment Block] %.*s\n", (int)(commentEnd - commentStart - 2), commentStart + 2);
            printf("Line: %d Token Text: @/ Token Type: Comment End\n", lineNumber);
            if (*after && strlen(after) > 0)
                tokenizeAndScanLine(after, lineNumber, &errorCount);
        } else if (commentStart) {
            // Comment starts but doesn't end on this line
            *commentStart = '\0';
            tokenizeAndScanLine(line, lineNumber, &errorCount);
            printf("Line: %d Token Text: /@ Token Type: Comment Start\n", lineNumber);
            strcat(commentBuffer, commentStart + 2);
            inMultilineComment = 1;
        } else {
            // No comment on this line
            tokenizeAndScanLine(line, lineNumber, &errorCount);
        }

        lineNumber++;
    }
    printf("\nTotal NO of errors: %d\n", errorCount);
    fclose(file);
}
