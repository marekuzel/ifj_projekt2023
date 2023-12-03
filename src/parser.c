#include <assert.h>
#include "scanner.h"
#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include "parser.h"
#include "code_gen.h"

void Parser_init(Parser_t *parser){
    assert(parser != NULL);

    parser->token_current = NULL;
    parser->token_topOfStack = NULL;
    parser->return_in_func = false;
    parser->find_id_type = false;
    parser->assign = false;
    parser->if_while = false;

    parser->stack = calloc(1,sizeof(Stack));
    CHECK_MEM_ERR(parser->stack)
    Stack_Init(parser->stack) ;

    parser->symtable = calloc(1,sizeof(symtable_t));
    CHECK_MEM_ERR(parser->symtable)
    table_init(parser->symtable);

    parser->buffer = calloc(1,sizeof(ParamBufferT));
    CHECK_MEM_ERR(parser->buffer);
    param_buffer_init(parser->buffer);
}

void parser_stashExtraToken(Parser_t *parser, TokenT *token){
    assert(parser != NULL);
    assert(token != NULL);

    parser->token_extraToken = token;
}

void parser_getNewToken(Parser_t *parser){
    assert(parser != NULL);

    Stack_Push(parser->stack, parser->token_current);
    parser->token_topOfStack = parser->token_current;
    if (parser->token_extraToken != NULL){
        parser->token_current = parser->token_extraToken;
        parser->token_extraToken = NULL;
    }
    else{
        parser->token_current = generate_token();
    }
}

//takes case parser->buffer = malloc(sizeof(ParamBufferT));
Error parser_createParam (Parser_t * parser){
    assert(parser != NULL);
    //dont touch this
    int top = parser->stack->topIndex;
    TokenT ** ptr = parser->stack->array;
    param_t* param = param_create(ptr[top-2]->value.str,ptr[top-3]->value.str, ptr[top]->type);
    table_insert_param(parser->buffer, param);
    return SUCCESS;
}

void parser_dtor(Parser_t * parser){
    table_dispose(parser->symtable);
    Stack_Dispose(parser->stack);
    param_buffer_detor(parser->buffer);
    parser->current_entry = NULL;
    free(parser);
}