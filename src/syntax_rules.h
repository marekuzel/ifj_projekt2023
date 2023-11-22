#ifndef RULES_H
#define RULES_H

#include "scanner.h"
#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include "parser.h"
#include <stdio.h>

typedef enum rule_ret{
    RULE_SUCCESS,
    RULE_FAIL,
}rule_ret_t;

rule_ret_t parser_rule_id(Parser_t *)

rule_ret_t parser_rule_funcID(Parser_t *)

rule_ret_t parser_rule_stmt(Parser_t *);

rule_ret_t parser_rule_stmtAssign(Parser_t *);

rule_ret_t parser_rule_defFunc(Parser_t *);

rule_ret_t parser_rule_elseF(Parser_t *);

rule_ret_t parser_rule_expr(Parser_t *);


#define CHECK_TOKEN_TYPE(parser, expected_type) \
    do { \
        if ((parser)->token_current->type != (expected_type)) { \
            return RULE_FAIL; \
        } \
    } while(0)

#define GET_NEXT_AND_CALL_RULE(parser, rule) \
    do { \
        Parser_getNewToken(parser); \
        if (parser->token_current->type == TOKEN_EOF) { \
            return RULE_FAIL; \
        } \
        parser_rule_rule(parser); \
    } while(0)

#define GET_NEXT_AND_CHECK_TYPE(parser, expected_type) \
    do { \
        Parser_getNewToken(parser); \
        CHECK_TOKEN_TYPE(parser, expected_type); \
    } while(0)
#endif