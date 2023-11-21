#include "utils.h"

#define MAX_LABEL 256

/**
 * 
*/
void gen_expr(ast_node_t *ast);

/**
 * 
*/
void gen_expr_binop(ast_node_t *ast);

/**
 * 
*/
void gen_expr_conv(ast_node_t *ast);

/**
 * 
*/
void gen_cond(ast_node_t *ast, const char *label_name, const int label_num);


void gen_statement(ast_node_t *ast);

void gen_string_op(ast_node_t *ast);

void gen_write(ast_node_t *ast);

void gen_read(ast_node_t *ast, const char *type);

void gen_statements(ast_node_t *ast);

void gen_loop(ast_node_t *ast);

void gen_cond_branch(ast_node_t *ast);

void gen_function_call(ast_node_t *ast);