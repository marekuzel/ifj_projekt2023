#include "scanner.h"
#include "errors.h"
#include "utils.h"
#include "symbolTable.h"
#include "parser.h"
#include "scanner.c"

void Parser_init(Parser_t *parser){
    parser->token_current = NULL;
    parser->token_topOfStack = NULL;
    Stack *stack = malloc(sizeof(Stack));
    Stack_Init(stack);
}

void Parser_getNewToken(Parser_t *parser){
    Stack *stack = parser->stack;
    Stack_Push(stack, *(parser)->token_current);
    parser->token_topOfStack = parser->token_current;
    parser->token_current = generate_token();
}

parser_ret_t Parser_dtor(Parser_t * parser){
    if (parser == NULL)
        return PARSER_DTOR_FAIL;
    Stack_Dispose(parser->stack);
    free(parser);
    return PARSER_DTOR_SUCCES;
}