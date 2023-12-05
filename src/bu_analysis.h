/**
 * Project: Compliler IFJ23 implementation 
 * File: bu_analysis.h
 * 
 * @brief bottom up analysis interface
 * 
 * @author Tímea Adamčíková xadamc09
*/

#include "errors.h"
#include "symtable.h"
#include <stdbool.h>

#ifndef BOTTOM_UP_ANALYSIS_H
#define BOTTOM_UP_ANALYSIS_H

#define NUM_OF_EXPR 14
#define MAX_EXP_LEN 5

typedef struct used_types {
    bool t_int;
    bool t_double;
    bool t_string;
    bool t_int_nil;
    int int_nil;
    bool t_double_nil;
    int double_nil;
    bool t_string_nil;
    int string_nil;
    bool t_nil;
} used_types_t;

/**
 * @brief bottom up analysis of expression (semantic and syntax)
 * 
 * @param next token which doesnt belong to expression
 * @param streamTokens stack including tokens
 * @param symTable symtable with variables
 * @param exprRetType final type of expression
 * @param if_while expresion in if/while condition
 * @return result of expression (semantic errors, syntax error or success)
*/
Error bu_read(TokenT** , Stack* streamTokens, symtable_t* symTable, TokenType* exprRetType, bool if_while);

/**
 * @brief generate expression
 * 
 * @param tokenStack terms to by generated
 * @param ruleStack used rules in expressiono
 * @param convert convert int to double
 * @param conc concatenate
 * @param symTable where to find if variable is global 
 * @return TYPE_COMPATIBILITY_ERROR when trying to convert variable
 * @return SUCCESS otherwise
*/
Error generate(Stack* tokenStack, stack_char_t* ruleStack, bool convert, bool conc, symtable_t* symTable);

/**
 * @brief check combination of rules in expression
 * 
 * @param stack stack of used rules
 * @param only_strings true if expression includes only string terms, false otherwise
 * @param typeNil true if expression includes nil
 * @param if_while true if expression is in if/while condition
 * @return TYPE_COMPATIBILITY_ERROR when rules doent match with used types
 * @return SUCCESS otherwise
*/
Error check_comb(stack_char_t* stack, bool only_strings, bool typeNil, bool if_while);

/**
 * @brief check combination of types in expression
 * 
 * @param tokenStack stack of used variables/literals
 * @param ruleStack stack of used rules
 * @param types used types in expression
 * @param division_types used division types
 * @param exprRetType final type of expression
 * @param symTable symtable with variables
 * @param if_while expresion in if/while condition
 * @return TYPE_COMPATIBILITY_ERROR when types cant be used in one expression
 * @return SUCCESS otherwise
*/
Error check_semantic(Stack* tokenStack, stack_char_t* ruleStack, used_types_t* types, used_types_t* division_types, TokenType** exprRetType, symtable_t* symTable, bool if_while);

/**
 * @brief assing char to be find in precedence table
 * 
 * @param symbol actual token
 * @param next set when token doesnt belog to expression
 * @param tokenStack stack for storing terms
 * @param types used types in expression
 * @param symbolRet symbol for precedence table
 * @param varCounter number of terms
 * @return UNDEFINED_VARIABLE_ERROR when variable is not found in symtable
 * @return SUCCESS otherwise
*/
Error check_symbol(TokenT* symbol, TokenT** next, Stack* tokenStack, used_types_t* types, symtable_t* symTable, char** symbolRet, int* varCounter);

/**
 * @brief find rule
 * 
 * @param stackRule rule from the top of stack
 * @param stack stack for storing expression
 * @param ruleStack stack for storing rules
 * @return SYNTAX_ERROR when rule doesnt exist
 * @return SUCCESS otherwise
*/
Error check_rule(char* stackRule, stack_char_t* stack, stack_char_t* ruleStack);

/**
 * @brief read from stack to find rule
 * 
 * @param stack stack for storing expression
 * @param ruleStack stack for storing rules
 * @return SYNTAX_ERROR when rule doesnt exist
 * @return SUCCESS otherwise
*/
Error find_rule(stack_char_t* stack, stack_char_t* ruleStack);

/**
 * @brief type of operand
 * 
 * @param symbol token
 * @param symTable symtable with variables
 * @param type type of token
 * @return UNDEFINED_VARIABLE_ERROR when variable is not found in symtable
 * @return SUCCESS otherwise
*/
Error type_in_special_expr(TokenT* symbol, symtable_t* symTable, TokenType* type);

/**
 * @brief check division types
 * 
 * @param prevprev first operand in divison
 * @param actual second operand in division
 * @param usedTypes types used in expression
 * @param symTable symtable with variables
 * @return TYPE_COMPATIBILITY_ERROR when not dividing int/int float/float
 * @return SUCCESS otherwise
*/
Error type_check_div(TokenT* prevprev, TokenT* actual, used_types_t* usedTypes, symtable_t* symTable);

/**
 * @brief check function call
 * 
 * @param token function name
 * @param symTable symtable with variables and functions
 * @param resType final type of expression
 * @param streamTokens stack including tokens
 * @return WRONG_NUM_TYPE_ERROR when parameters of funcion are wrong
 * @return UNDEFINED_VARIABLE_ERROR when parameter is not in symtable
 * @return SUCCESS otherwise
*/
Error deal_with_func(TokenT* token, symtable_t* symTable, TokenType** resType, Stack* streamTokens);

/**
 * @brief function in symtable
 * 
 * @param token function name
 * @param symTable symtable with variables and functions
 * @return UNDEFINED_FUNCTION_ERROR when function is not in symtable
 * @return SUCCESS otherwise
*/
Error isFunc(TokenT* token, symtable_t* symTable);

/**
 * @brief check types of ?? operator
 * 
 * @param prevprev first operand in divison
 * @param actual second operand in division
 * @param symTable symtable with variables
 * @return TYPE_COMPATIBILITY_ERROR when types are not compatible
 * @return SUCCESS otherwise
*/
Error type_check_2qm(TokenT* prevprev, TokenT* actual, symtable_t* symTable);

/**
 * @brief change variable type when operator !
 * 
 * @param tokenStack stack of variables in expression 
 * @param types used types in expression
*/
void change_type_withQM(Stack* tokenStack, used_types_t* types);

#endif
