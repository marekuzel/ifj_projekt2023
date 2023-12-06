/**
 * Project: Compliler IFJ23 implementation 
 * File: syntax_rules.h
 * 
 * @brief declaration of syntax rules 
 * 
 * @author Marek Kužel xkuzel11 
*/

#ifndef RULES_H
#define RULES_H

#include "scanner.h"
#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include "parser.h"
#include "bu_analysis.h"
/*
*****************************
Implementaion of syntax rules
*****************************
*/

/**
 * @brief rule to check if token is identifier
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if token is not identifier
 */
Error parser_rule_id(Parser_t *);

/**
 * @brief rule to check if token is identifier and function
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if token is not identifier and function in global scope
 */
Error parser_rule_funcID(Parser_t *);

/**
 * @brief rule to check syntax of a statement
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is incorrect
 */
Error parser_rule_stmt(Parser_t *);

/**
 * @brief rules to check syntax of value assignment to variable/constant
 * @param[in] parser
 * 
 * @return Error if syntax is incorrect
 */
Error parser_rule_stmtAssign(Parser_t *);

/**
 * @brief rule to check syntax of function definition
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_defFunc(Parser_t *);

/**
 * @brief rule to check syntax of function return block
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_funcRet(Parser_t *);

/**
 * @brief rule to check syntax of function parameters in definition
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_paramsDef(Parser_t *);

/**
 * @brief rule to recursively check syntax of function parameters in definition
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_paramsDefSeq(Parser_t *);

/**
 * @brief rules to check syntax of else block
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_elseF(Parser_t *);

/**
 * @brief rules to check type
 * @param[in] parser
 * 
 * @return Error SYNATX_ERROR if syntax is not correct
 */
Error parser_rule_type(Parser_t *);

/**
 * @brief recursive rule to check syntax of stmt ended with "}
 * @param[in] parser"
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_stmtSeq(Parser_t *);

/**
 * @brief recursive rule to check syntax of stmt ended with "}" and return token with expression
 * @param[in] parser"
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_stmtSeqRet(Parser_t *);

/**
 * @brief rule to check syntax of expression via bu_analysis
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_expr(Parser_t *);

/**
 * @brief recursive rule to check syntax of stmt ended with "}" and return token
 * @param[in] parser"
 * 
 * @return Error 
 */
Error parser_rule_stmtVoidSeqRet(Parser_t *);

/**
 * @brief rule to check syntax of function call
 * @param[in] parser
 * 
 * @return Error 
 */
Error parser_rule_callFunc(Parser_t *);

/**
 * @brief rule to check syntax of function call parameter
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_paramsCall (Parser_t*);

/**
 * @brief recursive rule to check syntax of function call parameter
 * @param[in] parser
 * 
 * @return Error 
 */
Error parser_rule_paramsCallSeq (Parser_t*);

/**
 * @brief main rule to check syntax of program
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_stmtMainSeq (Parser_t *);

/**
 * @brief rule to check syntax of function definition parameter name
 * @param[in] parser
 * 
 * @return Error SYNTAX_ERROR if syntax is not correct
 */
Error parser_rule_name (Parser_t *);

#define CHECK_TOKEN_TYPE(parser, expected_type) \
    do { \
        if ((parser)->token_current->type != (expected_type)) { \
            return SYNTAX_ERROR; \
        } \
    } while(0)

#define GET_NEXT_AND_CALL_RULE(parser, rule) \
    do { \
        parser_getNewToken(parser); \
        if (parser->token_current->type == TOKEN_EOF) { \
            return SYNTAX_ERROR; \
        } \
        RuleErr = parser_rule_##rule(parser); \
        if (RuleErr != SUCCESS) return RuleErr; \
    } while(0)

#define GET_NEXT_AND_CHECK_TYPE(parser, expected_type) \
    do { \
        parser_getNewToken(parser); \
        CHECK_TOKEN_TYPE(parser, expected_type); \
    } while(0)

#define RETURN_ERROR   \
    do{ \
        if (RuleErr != SUCCESS){ \
         return RuleErr; \
        } \
    }while(0)
#endif