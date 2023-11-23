#ifndef RULES_H
#define RULES_H

#include "scanner.h"
#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include "parser.h"
#include "bu_analysis.h"
#include <stdio.h>

Error parser_rule_id(Parser_t *);

Error parser_rule_funcID(Parser_t *);

Error parser_rule_stmt(Parser_t *);

Error parser_rule_stmtAssign(Parser_t *);

Error parser_rule_elseF(Parser_t *);

Error parser_rule_defFunc(Parser_t *);

Error parser_rule_funcRet(Parser_t *);

Error parser_rule_params(Parser_t *);

Error parser_rule_elseF(Parser_t *);

Error parser_rule_type(Parser_t *);

Error parser_rule_stmtSeq(Parser_t *);

Error parser_rule_stmtSeqRet(Parser_t *);

Error parser_rule_expr(Parser_t *);



#define CHECK_TOKEN_TYPE(parser, expected_type) \
    do { \
        if ((parser)->token_current->type != (expected_type)) { \
            return SYNTAX_ERROR; \
        } \
    } while(0)

#define GET_NEXT_AND_CALL_RULE(parser, rule) \
    do { \
        Parser_getNewToken(parser); \
        if (parser->token_current->type == TOKEN_EOF) { \
            return SYNTAX_ERROR; \
        } \
        return parser_rule_##rule(parser); \
    } while(0)

#define GET_NEXT_AND_CHECK_TYPE(parser, expected_type) \
    do { \
        Parser_getNewToken(parser); \
        CHECK_TOKEN_TYPE(parser, expected_type); \
    } while(0)
#endif