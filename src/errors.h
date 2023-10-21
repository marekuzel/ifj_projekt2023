typedef enum error_e {
    SUCCESS=0,
    LEXICAL_ERROR=1,
    SYNTAX_ERROR=2,
    UNDEFINED_FUNCTION_ERROR=3,          // undefined function, variable redefinition
    WRONG_NUM_TYPE_ERROR=4,              // wrong number/type of parameters when calling the function or wrong type of return value from function
    UNDEFINED_VARIABLE_ERROR=5,          // using an undefined or uninitialized variable
    MISSING_OVERRUN_EXPRESSION_ERROR=6,  // moving/overrun expression in the return statement from the function
    TYPE_COMPATIBILITY_ERROR=7,          // type compatibility semantic error in arithmetic, string, and relational expressions
    UNSPECIFIED_TYPE_ERROR=8,            // the variable or parameter type is not specified and cannot be inferred from the expression used
    ANOTHER_SEMANTIC_ERROR=9,            // other semantic errors
    INTERNAL_COMPILER_ERROR=99
} Error;