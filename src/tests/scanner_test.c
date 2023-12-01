#include <stdio.h>
#include "../scanner.h"
#include "../utils.h"

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


int main() {
    TokenT *next_token = generate_token();
    while (next_token->type != TOKEN_EOF) {
        print_token(next_token);
        // token_dtor(next_token);
        next_token = generate_token();
    }
    token_dtor(next_token);

}