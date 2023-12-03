#include "utils.h"
#include "scanner.h"
#include "errors.h"
#include "parser.h"
#include "symtable.h"
#include <stdio.h>


static const char *tokentype_to_string[] = {
    "TOKEN_ZERO",
    "TOKEN_IDENTIFIER",
    "TOKEN_DT_DOUBLE",
    "TOKEN_DT_DOUBLE_NIL",
    "TOKEN_DT_INT",
    "TOKEN_DT_INT_NIL",
    "TOKEN_DT_STRING",
    "TOKEN_DT_STRING_NIL",
    "TOKEN_LET",
    "TOKEN_VAR",
    "TOKEN_RETURN",
    "TOKEN_IF",
    "TOKEN_ELSE",
    "TOKEN_WHILE",
    "TOKEN_FUNC",
    "TOKEN_NIL",
    "TOKEN_OPERATOR",
    "TOKEN_STRING", 
    "TOKEN_INTEGER", 
    "TOKEN_DOUBLE",
    "TOKEN_ASSIGN",
    "TOKEN_L_BRACKET",
    "TOKEN_R_BRACKET",
    "TOKEN_LC_BRACKET",
    "TOKEN_RC_BRACKET",
    "TOKEN_COLON",
    "TOKEN_COMMA",
    "TOKEN_ARROW",
    "TOKEN_EOF"
};


static inline void print_token(TokenT *token) {
    if (token == NULL) {
        printf("Invalid input\n");
        return;
    }
    printf("TOKEN : ");
    switch (token->type) {
    case TOKEN_INTEGER:
        printf("%s %d",tokentype_to_string[TOKEN_INTEGER],token->value.i);
        break;
    case TOKEN_DOUBLE:
        printf("%s %a",tokentype_to_string[TOKEN_DOUBLE],token->value.d);
        break;
    default:
        printf("%s %s",tokentype_to_string[token->type],token->value.str);
        break;
    }
}


#define NEXT_TOKEN                      \
    token = generate_token();           \
    Stack_Push(parser->stack,token);    \
    print_token(token);                 \

#define CHECK_TOKEN(token_type)     \
    if(token->type != token_type){  \
        return SYNTAX_ERROR;        \
    }                               \

#define GET_NEXT_AND_CHEK_TOKEN(token_type)     \
    NEXT_TOKEN                                  \
    CHECK_TOKEN(token_type)                     \


#define ADD_AND_CHECK_PARAM                         \
    if (parser_createParam(parser) != SUCCESS) {    \
        return ANOTHER_SEMANTIC_ERROR;              \
    }                                               \

Error get_param_def(Parser_t *parser, symtable_entry_t *entry);
bool is_token_data_type(TokenT *token);
Error find_allFuncDef(Parser_t* parser);
