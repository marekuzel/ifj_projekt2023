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