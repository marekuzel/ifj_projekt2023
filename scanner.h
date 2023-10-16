#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>

#ifndef SCANNER_H
#define SCANNER_H

typedef enum tokentype_e {
    TOKEN_ZERO, // lze na nej inicializovat token
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_DATATYPE,
    TOKEN_STRING, 
    TOKEN_INTEGER, 
    TOKEN_DECIMAL,
    TOKEN_TERM,
    TOKEN_LINE_COMMENT,
    TOKEN_BLOCK_COMMENT,
    TOKEN_EOF
} TokenType;

typedef enum fsm_state_e {
    STATE_START,
    STATE_TEXT,
    STATE_SLASH,
    STATE_LINE_COMMENT,
    STATE_BLOCK_COMMENT,
    STATE_END_BLOCK_COMMENT,
    STATE_STRING,
    STATE_MULTILINE_STRING,
    STATE_TWO_DOUBLE_QUOTES
} ScannerState;

typedef struct tok {
    TokenType type;
    char* value;
    struct token_t* next;
} token_t;

#define MAX_STRLEN 255 // NENI OMEZENA, VYRESIT !!
#define NOF_KEYWORDS 8
#define NOF_DATATYPES 3

char keywords[NOF_KEYWORDS][10] = {
    "else", "func", "if", "let", "nil", "return", "var", "while"
};

char datatypes[NOF_DATATYPES][10] = {
    "Double", "Int", "String"
};

#endif