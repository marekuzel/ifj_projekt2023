#include "../parser.h"
#include "../syntax_rules.h"
#include <stdio.h>
#include <assert.h>

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


void print_token(TokenT *token) {
    if (token == NULL) {
        printf("Invalid input\n");
        return;
    }
    printf("TOKEN : ");
    switch (token->type) {
    case TOKEN_INTEGER:
        printf("%s %d\n",tokentype_to_string[TOKEN_INTEGER],token->value.i);
        break;
    case TOKEN_DOUBLE:
        printf("%s %lf\n",tokentype_to_string[TOKEN_DOUBLE],token->value.d);
        break;
    default:
        printf("%s %s\n",tokentype_to_string[token->type],token->value.str);
        break;
    }
}

#define TEST(name)                              \
    Parser_t* parser;                           \
    parser = malloc(sizeof(Parser_t));            \
    if (parser_init(parser) == INTERNAL_COMPILER_ERROR){ \
        fprintf (stderr, "parser init error\n"); \
    } \
    inset_test_label(name);                     \

#define END_TEST(name)               \
    printf ("\n-----------------\n-----------------\ntest_%s successful\n-----------------\n-----------------\n", name); \

void inset_test_label(char *test_name) {
    printf("\n##########test_%s########\n\n",test_name);
}

void test_1(){
    TEST("Test declaration of variable\n");
    Error err = parser_rule_stmtMainSeq(parser);
    if (err==SYNTAX_ERROR){
        print_token(parser->token_current);
        // print_token(parser->stack->array[parser->stack->topIndex]);
        printf("error\n");
    }
    parser_dtor(parser);
    END_TEST("Test declaration of variable\n");
}

int main(){
    test_1(); //test for let assignment
}