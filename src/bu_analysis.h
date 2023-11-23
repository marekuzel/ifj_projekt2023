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
} used_types_t;

Error bu_read(TokenT** next);

#endif
