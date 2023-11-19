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

    Stack *stack;

    //global symtable
    //local symtable
}Parser_t;

typedef enum {
    PARSER_DTOR_SUCCES,
    PARSER_DTOR_FAIL,
    PARSER_VALID_TOKEN,
    PARSER_INVALID_TOKEN,
    } parser_ret_t;

void Parser_init();

parser_ret_t Parser_dtor(Parser_t*);

void Parser_getNewToken(Parser_t *parser){

typedef enum{
    PARSER_RULE_STATEMENT,
    PARSER_RULE_FAIL,
} parser_rule_t;

#endif