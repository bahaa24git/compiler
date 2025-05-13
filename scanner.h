//
// Created by USER on 5/13/2025.
//

#ifndef SCANNER_H
#define SCANNER_H
typedef struct {
    char *keyword;
    char *tokenType;
} Keyword;
void scanFile(const char *filename);
#endif //SCANNER_H
