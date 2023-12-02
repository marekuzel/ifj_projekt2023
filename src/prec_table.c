#include "prec_table.h"
#include "errors.h"
#include <string.h>
#include <stdio.h>

char precTable[PREC_TABLE_SIZE][PREC_TABLE_SIZE] = {
   // +    -    *    /    (    )    i    $    ==  !=    <   <=    >   >=   ??
    {'>', '>', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<'}, // +
    {'>', '>', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<'}, // -
    {'>', '>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<'}, // *
    {'>', '>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<'}, // /
    {'<', '<', '<', '<', '<', '=', '<', ' ', '<', '<', '<', '<', '<', '<', '<'}, // (
    {'>', '>', '>', '>', ' ', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>'}, // )
    {'>', '>', '>', '>', ' ', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>'}, // i
    {'<', '<', '<', '<', '<', ' ', '<', ' ', '<', '<', '<', '<', '<', '<', '<'}, // $
    {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // ==
    {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // !=
    {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // <
    {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // <=
    {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // >
    {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // >=
    {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' '}  // ??
};

Error op_to_symbol(char *op, PrecSymbol* symbol) {
    if (!strcmp(op, "+")) {
        *symbol = PREC_SYMBOL_PLUS;
    } else if (!strcmp(op, "-")) {
        *symbol = PREC_SYMBOL_MINUS;
    } else if (!strcmp(op, "*")) {
        *symbol = PREC_SYMBOL_MUL;
    } else if (!strcmp(op, "/")) {
        *symbol = PREC_SYMBOL_DIV;
    } else if (!strcmp(op, "(")) {
        *symbol = PREC_SYMBOL_LEFTB;
    } else if (!strcmp(op, ")")) {
        *symbol = PREC_SYMBOL_RIGHTB;
    } else if (!strcmp(op, "i")) {
        *symbol = PREC_SYMBOL_ID;
    } else if (!strcmp(op, "$")) {
        *symbol = PREC_SYMBOL_END;
    } else if (!strcmp(op, "==")) {
        *symbol = PREC_SYMBOL_EQ;
    } else if (!strcmp(op, "!=")) {
        *symbol = PREC_SYMBOL_NEQ;
    } else if (!strcmp(op, "<")) {
        *symbol = PREC_SYMBOL_LT;
    } else if (!strcmp(op, "<=")) {
        *symbol = PREC_SYMBOL_LTE;
    } else if (!strcmp(op, ">")) {
        *symbol = PREC_SYMBOL_GT;
    } else if (!strcmp(op, ">=")) {
        *symbol = PREC_SYMBOL_GTE;
    } else if (!strcmp(op, "??")) {
        *symbol = PREC_SYMBOL_NULLISH;
    } else {
        fprintf(stderr, "ERROR in op_to_symbol in prec_table.c file: wrong value %s\n", op);
        return SYNTAX_ERROR; // TODO check me
    }

    return SUCCESS;
}


Error get(char* leftOperator, char* rightOperator, PrecAction* act) {
    PrecSymbol left, right;

    Error result = op_to_symbol(leftOperator, &left);
    if (result != SUCCESS) {
        return result;
    } 

    result = op_to_symbol(rightOperator, &right);
    if (result != SUCCESS) {
        return result;
    } 
    
    if (precTable[left][right] == '<') {
        *act = PREC_ACTION_SHIFT;
    } else if (precTable[left][right] == '>') {
        *act = PREC_ACTION_REDUCE;
    } else if (precTable[left][right] == '=') {
        *act = PREC_ACTION_EQ;
    } else {
        *act = PREC_ACTION_ERROR;
    }

    return SUCCESS;
}
