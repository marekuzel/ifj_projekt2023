#include "../parser.h"
#include "../syntax_rules.h"
#include <stdio.h>
#include <assert.h>

#define TEST(name)                              \
    Parser_t* parser;                           \
    parser = malloc(sizeof(Parser_t));            \
    if (parser_init(parser) == INTERNAL_COMPILER_ERROR){ \
        fprintf (stderr, "parser init error\n"); \
    } \
    inset_test_label(name);                     \

#define END_TEST(name)               \
    printf ("-------------test_%s successful-------------\n", name); \

void inset_test_label(char *test_name) {
    printf("\n##########test_%s########\n\n",test_name);
}

void test_1(){
    TEST("Test declaration of variable\n");
    Error err = parser_rule_stmtMainSeq(parser);
    if (err==SYNTAX_ERROR){
        printf("error\n");
        printf ("%s\n", parser->token_current->);
    }
    END_TEST("Test declaration of variable\n");
}

int main(){
    test_1(); //test for let assignment
}