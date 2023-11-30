#ifdef RUN_TESTS 
#else
#include "scanner.h"
#endif

#include "errors.h"
#include "utils.h"
#include "symtable.h"
#include "parser.h"


void Parser_init(Parser_t *parser){
    parser->token_current = NULL;
    parser->token_topOfStack = NULL;
    Stack *stack = malloc(sizeof(Stack));
    Stack_Init(stack);
    symtable_t *symtable = malloc(sizeof(symtable_t));
    table_init(symtable);
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

Error parser_initLocalSymtable(Parser_t *parser){
    table_add_scope(parser->symtable);
    parser->buffer = malloc(sizeof(ParamBufferT));
    if (parser->buffer == NULL) return INTERNAL_COMPILER_ERROR;
    return SUCCESS;
}


void parser_closeLocalSymtable(Parser_t *parser){
    table_remove_scope(parser->symtable);
    param_buffer_detor(parser->buffer); 
    free(parser->buffer); //TODO: unsure if neccesary
}

void parser_dtor(Parser_t * parser){
    table_dispose(parser->symtable);
    param_buffer_detor(parser->buffer);
    Stack_Dispose(parser->stack);
    parser->current_entry = NULL;
    free(parser);
}