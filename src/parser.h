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
    char* current_function;
    char* current_id;
    bool return_in_func;
    bool find_id_type;
    bool assign;
    bool if_while;
    Stack *stack;
    ParamBufferT *buffer;
    symtable_t *symtable;
    symtable_entry_t *current_entry;
}Parser_t;

typedef enum {
    PARSER_DTOR_SUCCES,
    PARSER_DTOR_FAIL,
    PARSER_VALID_TOKEN,
    PARSER_INVALID_TOKEN,
    } parser_ret_t;

void parser_init(Parser_t *);

void parser_dtor(Parser_t*);

void parser_getNewToken(Parser_t *);

void parser_stashExtraToken(Parser_t *, TokenT *);

void parser_symtableEntry(Parser_t *);

Error parser_initLocalSymtable(Parser_t *);

Error parser_createParam (Parser_t *);

#endif