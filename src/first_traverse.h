#include "utils.h"
#include "scanner.h"
#include "errors.h"
#include "parser.h"
#include "symtable.h"
#include <stdio.h>



#define NEXT_TOKEN                      \
    token = generate_token();           \
    if (token == NULL) {                \
        return LEXICAL_ERROR;           \
    }                                   \
    Stack_Push(parser->stack,token);    \

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
