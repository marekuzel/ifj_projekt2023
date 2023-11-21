#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

buff_ret_t buffer_init(BufferT *buffer) {
    buffer->length = 0;
    buffer->cap = BUFFER_CAP_S;

    buffer->bytes = (char *) calloc(buffer->cap,1);
    
    if (buffer->bytes == NULL) 
        return BUFF_INIT_FAIL;

    return BUFF_INIT_SUCCES;
}


buff_ret_t buffer_append(BufferT *buffer, const char chr) {
    if (buffer->length >= buffer->cap) {
        char *new_buff = realloc(buffer->bytes,buffer->cap * 2); 

        if (new_buff == NULL)
        {
            buffer_detor(buffer);
            return BUFF_APPEND_FAIL;
        }
        
        buffer->bytes = new_buff;
        buffer->cap *= 2;
    }

    buffer->bytes[buffer->length++] = chr;
    return BUFF_APPEND_SUCCES;  

}

void buffer_clear(BufferT *buffer) {
    buffer->length = 0;
}

char *buffer_export(const BufferT *buffer) {
    char * dst = (char * ) calloc(1,buffer->length+1);

    if (dst == NULL)
        return NULL;

    memcpy(dst,buffer->bytes,buffer->length);
    
    return dst;
}

void buffer_detor (BufferT *buffer) {
    free(buffer->bytes);
    buffer->bytes = NULL;
    buffer->cap = 0;
    buffer->length = 0;
}



stack_ret_t Stack_Init(Stack *stack) {
	if (stack == NULL){
		return STACK_INIT_FAIL;
	}
	stack->array = calloc(STACK_SIZE,sizeof(TokenT));
	if (stack->array == NULL){
		return STACK_INIT_FAIL;
	}
	stack -> topIndex = -1;
    return STACK_INIT_SUCCES;
}

bool Stack_IsEmpty(const Stack *stack) {
	return stack->topIndex == -1;
}

bool Stack_IsFull(const Stack *stack) {
		return stack->topIndex == STACK_SIZE-1;
}

void Stack_Top( const Stack *stack, TokenT **dataPtr ) {
    if (!Stack_IsEmpty(stack)){
        *dataPtr = stack->array[stack->topIndex];
    }
}

stack_ret_t Stack_Pop( Stack *stack) {
	if (!Stack_IsEmpty(stack)){
		stack->topIndex--;
        return STACK_POP_SUCCES;
	}
    return STACK_POP_FAIL;
}

stack_ret_t Stack_Push( Stack *stack, TokenT *data ) {
	if (!Stack_IsFull(stack)) {
		stack->topIndex++;
		stack->array[stack->topIndex] = data;
        return STACK_PUSH_SUCCES;
	}
	else{
		return STACK_PUSH_FAIL;
	}
}

void Stack_Dispose( Stack *stack ) {
	stack->topIndex = -1;
	free(stack->array);
	stack->array = NULL;
}


token_ret_t token_init(TokenT *token,TokenType type, BufferT *buff) {
    token->type = type;
    char *token_value = buffer_export(buff);

    if (token_value == NULL) {
        return VALUE_ASSIGNMENT_FAIL;
    }

    if (type == TOKEN_INTEGER) {
        token->value.i = (int) strtol(token_value,NULL,10);
        return INT_CONVERSION_SUCCES;

    }
    else if (type == TOKEN_DECIMAL) {
        token->value.d = strtod(token_value,NULL);
        return DOUBLE_CONVERTION_SUCCES;
    }

    buffer_detor(buff);

    token->value.str = token_value;
    return VALUE_ASSIGNMENT_SUCCES;

}

void print_Token(TokenT *token){
    switch(token->type) {
        case TOKEN_INTEGER:
            printf("%d\n",token->value.i);
            break;
        case TOKEN_DECIMAL:
            printf("%f\n",token->value.d);
            break;
        default:
            printf("%s\n",token->value.str);
            break;
    }
}

void token_dtor(TokenT *token) {
    free(token->value.str);
    token->value.str = NULL;
    free(token);
}


ast_node_t *create_leaf(char *identifier, litValue value, ast_node_type_t val_type) {

    New_node(new_leaf);
    
    if (identifier != NULL) {
        new_leaf->identifier = identifier;
        new_leaf->node_type = VARIABLE;
    } else {
        new_leaf->value = value;
        new_leaf->node_type = val_type;
    }

    return new_leaf;   
}


ast_node_t *create_bin_op(char operator, ast_node_t *left, ast_node_t *right) {

    New_node(new_binop);

    new_binop->operator = operator;
    new_binop->left = left;
    new_binop->right = right;
    new_binop->node_type = BINARY_OPERATOR;

    return new_binop;

}

ast_node_t *create_conversion(conv_type_t conv_type, ast_node_t* to_convert) {
    
  New_node(new_conversion);

    new_conversion->child = to_convert;
    new_conversion->node_type = CONVERSION;
    new_conversion->conversion_type = conv_type;

    return new_conversion;

}

ast_node_t *create_cond(rel_op_t rel_op, ast_node_t *left, ast_node_t *right) {
    
    New_node(new_cond);

    new_cond->relation_operator = rel_op;
    new_cond->left = left;
    new_cond->right = right;
    new_cond->node_type = CONDITION;

    return new_cond;
}


ast_node_t *create_if_branch(ast_node_t *condition, ast_node_t *statemests_if, ast_node_t* statements_else) {

    New_node(new_if);

    new_if->left = condition;
    new_if->next = statemests_if;

    if (statements_else != NULL) {
        new_if->right = statements_else;
        new_if->has_else = true;
    }

    new_if->node_type = IF_ELSE;

    return new_if;
}


ast_node_t *create_loop(ast_node_t *condition, ast_node_t *statemets) {

    New_node(new_loop);

    new_loop->left = condition;
    new_loop->right = statemets;
    new_loop->node_type = WHILE_LOOP;

    return new_loop;
}

ast_node_t *create_expr(ast_node_t *expr) {

    New_node(new_expr);

    new_expr->child = expr;
    new_expr->node_type = EXPRESION;

    return new_expr;
}


ast_node_t *create_prog(ast_node_t *statemts) {

    New_node(new_prog);

    new_prog->next = statemts;
    new_prog->node_type = PROGRAM;

    return new_prog;

}



void ast_add_statement(ast_node_t *parrent_node, ast_node_t *statement) {

    if (parrent_node->last == NULL) {
        parrent_node->next = statement;
    } else {
        parrent_node->last->next = statement;
    }

    parrent_node->last = statement;

}


void ast_dispose(ast_node_t *ast) {

    if (ast == NULL)
        return;

    ast_dispose(ast->left);
    ast_dispose(ast->right);
    ast_dispose(ast->child);
    ast_dispose(ast->next);

    free(ast->value.str);
    free(ast->identifier);
    free(ast);
    
}