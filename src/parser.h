#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include <stdio.h>

typedef struct Parser {
    TokenT *token_current;
    TokenT *token_topOfStack;
    TokenT *token_extraToken; //in rule expr we need to take out of scanner one more token, so we will store it here

    Stack *stack;
    symtable_t *symtable;
}Parser_t;

typedef enum {
    PARSER_DTOR_SUCCES,
    PARSER_DTOR_FAIL,
    PARSER_VALID_TOKEN,
    PARSER_INVALID_TOKEN,
    } parser_ret_t;

void parser_init(Parser_t *parser);

void parser_dtor(Parser_t*);

void parser_getNewToken(Parser_t *parser);

Error parser_addLocalSymtable(Parser_t *parser);

void parser_stashExtraToken(Parser_t *parser, TokenT *);

#endif