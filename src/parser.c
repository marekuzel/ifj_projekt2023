#include "scanner.h"
#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include "parser.h"


Error Parser_init(Parser_t *parser){
    parser->token_current = NULL;
    parser->token_topOfStack = NULL;

    parser->stack = malloc(sizeof(Stack));
    if (!(parser->stack)) return INTERNAL_COMPILER_ERROR; 
    if (Stack_Init(parser->stack) == STACK_INIT_FAIL) return INTERNAL_COMPILER_ERROR;

    parser->symtable = malloc(sizeof(symtable_t));
    if (!(parser->symtable)) return INTERNAL_COMPILER_ERROR;
    table_init(parser->symtable);
}

void parser_stashExtraToken(Parser_t *parser, TokenT *token){
    parser->token_extraToken = token;
}

void parser_getNewToken(Parser_t *parser){
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

void parser_dtor(Parser_t * parser){
    table_dispose(parser->symtable);
    Stack_Dispose(parser->stack);
    parser->current_entry = NULL;
    free(parser);
}