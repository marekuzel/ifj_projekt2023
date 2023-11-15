#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>

#ifndef SCANNER_H
#define SCANNER_H

typedef enum fsm_state_e {
    STATE_START,
    STATE_TEXT,
    STATE_SLASH,
    STATE_LINE_COMMENT,
    STATE_BLOCK_COMMENT,
    STATE_END_BLOCK_COMMENT,
    STATE_STRING,
    STATE_MULTILINE_STRING,
    STATE_TWO_DOUBLE_QUOTES,
    STATE_EQUALS,
    STATE_EXCLAMATION,
    STATE_QUESTION,
    STATE_RELATIONAL_OPERATOR
} ScannerState;

#define NOF_KEYWORDS 8
#define NOF_DATATYPES 3

char keywords[NOF_KEYWORDS][10] = {
    "else", "func", "if", "let", "nil", "return", "var", "while"
};

char datatypes[NOF_DATATYPES][10] = {
    "Double", "Int", "String"
};

#endif