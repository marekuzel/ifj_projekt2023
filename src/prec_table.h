#ifndef IFJ__PREC_TABLE_H__
#define IFJ__PREC_TABLE_H__

#include "errors.h"

#define PREC_TABLE_SIZE 15
// char precTable[TABLE_SIZE][TABLE_SIZE] = {
//    // +    -    *    /    (    )    i    $    ==  !=    <   <=    >   >=
//     {'>', '>', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'}, // +
//     {'>', '>', '<', '<', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'}, // -
//     {'>', '>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'}, // *
//     {'>', '>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>'}, // /
//     {'<', '<', '<', '<', '<', '=', '<', ' ', '<', '<', '<', '<', '<', '<'}, // (
//     {'>', '>', '>', '>', ' ', '>', ' ', '>', '>', '>', '>', '>', '>', '>'}, // )
//     {'>', '>', '>', '>', ' ', '>', ' ', '>', '>', '>', '>', '>', '>', '>'}, // i
//     {'<', '<', '<', '<', '<', ' ', '<', ' ', '<', '<', '<', '<', '<', '<'}, // $
//     {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' '}, // == 
//     {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' '}, // !=
//     {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' '}, // <
//     {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' '}, // <=
//     {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' '}, // >
//     {'<', '<', '<', '<', '<', '>', '<', '>', ' ', ' ', ' ', ' ', ' ', ' '}  // >=
// };

typedef enum {
    PREC_SYMBOL_PLUS,   // +
    PREC_SYMBOL_MINUS,  // -
    PREC_SYMBOL_MUL,    // *
    PREC_SYMBOL_DIV,    // /
    PREC_SYMBOL_LEFTB,  // (
    PREC_SYMBOL_RIGHTB, // )
    PREC_SYMBOL_ID,     // i
    PREC_SYMBOL_END,    // $
    PREC_SYMBOL_EQ,     // ==
    PREC_SYMBOL_NEQ,    // !=
    PREC_SYMBOL_LT,     // <
    PREC_SYMBOL_LTE,    // <=
    PREC_SYMBOL_GT,     // >
    PREC_SYMBOL_GTE,    // >=
    PREC_SYMBOL_NULLISH,// ??
} PrecSymbol;

typedef enum {
    PREC_ACTION_SHIFT,
    PREC_ACTION_REDUCE,
    PREC_ACTION_EQ,
    PREC_ACTION_ERROR
} PrecAction;

/*
*
*/
Error get(char* leftOperator, char* rightOperator, PrecAction* act);

#endif