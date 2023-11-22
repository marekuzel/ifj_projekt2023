#include "utils.h"

typedef enum conv_type {
    FI,
    IF,
    IC,
    CI,
} conv_type_t;

typedef enum rel_op {
    LT,
    LTE,
    GT,
    GTE,
    EQ,
    NEQ
} rel_op_t;


typedef enum lit_type { 
    INT_T, 
    STRING_T,
    DOUBLE_T,
    NILL
} lit_type_t;


/**
 * 
*/
void gen_expr_binop(const char operator);

/**
 * 
*/
void gen_expr_conv(conv_type_t conversion_type);

/**
 * 
*/
void gen_cond(rel_op_t relation_operator);

void push_var(char *id, bool global);
void push_literal(litValue value, lit_type_t type);
void clear_expr_stack();

// void gen_statement(ast_node_t *ast);

// void gen_string_op(ast_node_t *ast);

// void gen_write(ast_node_t *ast);

// void gen_read(ast_node_t *ast, const char *type);

// void gen_statements(ast_node_t *ast);

// void gen_loop(ast_node_t *ast);

// void gen_cond_branch(ast_node_t *ast);

// void gen_function_call(ast_node_t *ast);