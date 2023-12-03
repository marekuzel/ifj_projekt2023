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

Error bu_read(TokenT** next, symtable_t* symTable, TokenType* exprRetType, bool if_while);

#endif
