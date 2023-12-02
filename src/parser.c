#include "scanner.h"
#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include "parser.h"


Error parser_init(Parser_t *parser){
    parser->token_current = NULL;
    parser->token_topOfStack = NULL;
    parser->return_in_func = false;
    parser->find_id_type = false;
    parser->assign = false;

    parser->stack = malloc(sizeof(Stack));
    if (!(parser->stack)) return INTERNAL_COMPILER_ERROR; 
    if (Stack_Init(parser->stack) == STACK_INIT_FAIL) return INTERNAL_COMPILER_ERROR;

    parser->symtable = malloc(sizeof(symtable_t));
    if (!(parser->symtable)) return INTERNAL_COMPILER_ERROR;
    table_init(parser->symtable);
    return SUCCESS;
}

void parser_stashExtraToken(Parser_t *parser, TokenT *token){
    parser->token_extraToken = token;
}

Error parser_getNewToken(Parser_t *parser){
    if (Stack_Push(parser->stack, parser->token_current) == STACK_PUSH_FAIL) return INTERNAL_COMPILER_ERROR;
    parser->token_topOfStack = parser->token_current;
    if (parser->token_extraToken != NULL){
        parser->token_current = parser->token_extraToken;
        parser->token_extraToken = NULL;
    }
    else{
        parser->token_current = generate_token();
    }
    return SUCCESS;
}

//takes case parser->buffer = malloc(sizeof(ParamBufferT));
Error parser_createParam (Parser_t * parser){
    //dont touch this
    int top = parser->stack->topIndex;
    TokenT ** ptr = parser->stack->array;
    param_t* param = param_create(ptr[top-2]->value.str,ptr[top-3]->value.str, ptr[top]->type);
    if (param == NULL) return INTERNAL_COMPILER_ERROR;
    table_insert_param(parser->buffer, param);
    return SUCCESS;
}

void parser_dtor(Parser_t * parser){
    table_dispose(parser->symtable);
    Stack_Dispose(parser->stack);
    parser->current_entry = NULL;
    free(parser);
}