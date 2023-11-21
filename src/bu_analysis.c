#include "utils.h"
#include "prec_table.h"
#include "bu_analysis.h"
#include "code_gen.h"
#include "errors.h"
#include "scanner.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char* check_symbol(TokenT* symbol, TokenT** next) {
    if (symbol->type == TOKEN_EOF || symbol->type == TOKEN_KEYWORD || symbol->type == TOKEN_DATATYPE || 
    symbol->type == TOKEN_LEFT_CURLY_BRACKET || symbol->type == TOKEN_RIGHT_CURLY_BRACKET ||
    symbol->type == TOKEN_COLON || symbol->type == TOKEN_COMMA) {
        *next = symbol;
        return "$";
    }

    if (symbol->type == TOKEN_IDENTIFIER || symbol->type == TOKEN_STRING || symbol->type == TOKEN_INTEGER ||
    symbol->type == TOKEN_DECIMAL) {
        return "i";
    }

    return (symbol->value.str);
}

void gen_code(char* stackRule) {
    if(!strcmp(stackRule, "i")) {
        push_var("a", false);
    } else if (!strcmp(stackRule, "E+E")) {
        gen_expr_binop('+');
    } else if (!strcmp(stackRule, "E*E")) {
        gen_expr_binop('*');
    }
}

Error check_rule(char* stackRule, stack_char_t* stack) {
    char expr[NUM_OF_EXPR][MAX_EXP_LEN] = {
    "i", "(E)", "E+E", "E-E", "E*E", "E/E", "E==E", "E!=E", "E<E", "E<=E", "E>E", "E>=E", "E??E", "E!"
    };

    for (int i = 0; i < NUM_OF_EXPR; i++) {
        if (!strcmp(stackRule, expr[i])) {
            printf("rule: %s\n", expr[i]);
            stack_char_push(stack, "E");
            gen_code(stackRule);
            return SUCCESS;
        }
    }

    return SYNTAX_ERROR;
}

Error find_rule(stack_char_t* stack) {
    stack_char_t tmp;
    stack_char_init(&tmp);

    while (!stack_char_empty(stack)) {
        char* stackTop = stack_char_top(stack);

        if (!strcmp(stackTop, "<")) {
            stack_char_pop(stack);
            char* stackChar = stack_char_top(&tmp);
            char* stackRule = (char*)malloc(5*sizeof(char));
            strcpy(stackRule, stackChar);
            stack_char_pop(&tmp);

            while (!stack_char_empty(&tmp)) {
                stackChar = stack_char_top(&tmp);
                strcat(stackRule, stackChar);
                stack_char_pop(&tmp);
            }
            Error err = check_rule(stackRule, stack);
            return err;
        } else {
            stack_char_push(&tmp, stackTop);
            stack_char_pop(stack);
        }
    }

    return SYNTAX_ERROR;
}

// TODO (lok symtable) 
Error bu_read(TokenT** next) {
    stack_char_t stack;
    stack_char_init(&stack);
    stack_char_push(&stack, "$");

    char* stackTerminal = stack_char_top(&stack);
    TokenT* token = generate_token();
    char* symbol = check_symbol(token, next);
    PrecAction action;
    Error err = get(stackTerminal, symbol, &action);

    while (strcmp(symbol, "$") != 0 || stack_char_2oftop(&stack) == false) {
        if (err == SUCCESS) {
            switch(action) {
                case PREC_ACTION_SHIFT:
                    err = stack_insertAfterTerminal(&stack);
                    if (err == SYNTAX_ERROR) {
                        return SYNTAX_ERROR;
                    }
                    stack_char_push(&stack, symbol);

                    if ((*next)->type == TOKEN_ZERO) {
                        token = generate_token();
                        symbol = check_symbol(token, next);
                    }
                    break;
                case PREC_ACTION_REDUCE:
                    err = find_rule(&stack);

                    if (err == SYNTAX_ERROR) {
                        return SYNTAX_ERROR;
                    }
                    break;
                case PREC_ACTION_EQ:
                    stack_char_push(&stack, symbol);

                    if ((*next)->type == TOKEN_ZERO) {
                        token = generate_token();
                        symbol = check_symbol(token, next);
                    }

                    break;
                case PREC_ACTION_ERROR:
                    return SYNTAX_ERROR;
            }
        } else {
            return err;
        }
        stack_topTerminal(&stack, &stackTerminal);
        err = get(stackTerminal, symbol, &action);
    }

    return SUCCESS;
}

int main() {
    TokenT *next = malloc(sizeof(TokenT));
    next->type = TOKEN_ZERO;
    Error err = bu_read(&next);
    return 0;
}