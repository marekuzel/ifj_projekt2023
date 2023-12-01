#ifdef RUN_TESTS 
#else
#include "scanner.h"
#endif

#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include "parser.h"

//TODO: add return value
void Parser_init(Parser_t *parser){
    parser->token_current = NULL;
    parser->token_topOfStack = NULL;
    Stack *stack = malloc(sizeof(Stack));
    parser->buffer = malloc(sizeof(ParamBufferT));
    Stack_Init(stack);
    symtable_t *symtable = malloc(sizeof(symtable_t));
    table_init(symtable);
}

void parser_stashExtraToken(Parser_t *parser, TokenT *token){
    parser->token_extraToken = token;
}

void parser_getNewToken(Parser_t *parser){
    parser->token_topOfStack = parser->token_current;
    Stack_Push(parser->stack, parser->token_current);
    if (parser->token_extraToken != NULL){
        parser->token_current = parser->token_extraToken;
        parser->token_extraToken = NULL;
    }
    else{
        parser->token_current = generate_token();
    }
}
//takes case ( [name] [id] : [type] [parameters_CallSeq]*
//therefore, we know order of tokens on the stack and can manually take out tokens
//!!!!DO NOT USE INA ANY OTHER CASE !!!!!
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
    param_buffer_detor(parser->buffer);
    Stack_Dispose(parser->stack);
    parser->current_entry = NULL;
    free(parser);
}