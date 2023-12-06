/**
 * Project: Compliler IFJ23 implementation 
 * File: parser.h
 * 
 * @brief declaration of functions for syntax analysis
 * 
 * @author Marek Kužel xkuzel11
*/

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
    bool assign_to_double;
    bool assign_type_expr;
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

/**
 * @brief initialize parser structure
 * @param[in] parser
 */
void parser_init(Parser_t *);

/**
 * @brief clean up after parser structure
 * @param[in] parser
 * 
 */
void parser_dtor(Parser_t*);

/**
 * @brief gets new token from stack or stash
 * 
 * @param[in] parser
 * @return SUCCESS if token is valid
 * @return SYNTAX_ERROR if token is invalid
 */
Error parser_getNewToken(Parser_t *);

/**
 * @brief stash token to extra token
 * @param[in] parser
 * @param[in] token
 */
void parser_stashExtraToken(Parser_t *, TokenT *);

/**
 * @brief adds symtable entry to symtable
 * @param[in] parser
 */
void parser_symtableEntry(Parser_t *);

/**
 * @brief initialize local symtable
 * @param[in] parser
 * 
 * @return Error 
 */
Error parser_initLocalSymtable(Parser_t *);

/**
 * @brief creates param for function
 * @param[in] parser
 * 
 * @return Error 
 */
Error parser_createParam (Parser_t *);

#endif