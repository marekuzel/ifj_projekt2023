#include "utils.h"
#include "prec_table.h"
#include "bu_analysis.h"
#include "code_gen.h"
#include "symtable.h"
#include "errors.h"
#include "scanner.h"
#include <stdio.h>
#include <stdbool.h>

Error generate(Stack* tokenStack, stack_char_t* ruleStack, bool convert, bool double_div, bool conc) {
    // "i", "(E)", "E+E", "E-E", "E*E", "E/E", "E==E", "E!=E", "E<E", "E<=E", "E>E", "E>=E", "E??E"
    while (!stack_char_empty(ruleStack)) {
        char* rule = stack_bottom_read(ruleStack);
        TokenT* token;

        if(!strcmp(rule, "i")) {
            token = stack_read_token_bottom(tokenStack);

            if (token->type == TOKEN_DT_DOUBLE || token->type == TOKEN_DT_INT || token->type == TOKEN_DT_STRING) {
                gen_push_var(token->value.str, true);
            } else {
                // gen_push_lit(token->value, token->value); // right one after updated gen 
                switch (token->type) { // tmp
                    case TOKEN_INTEGER:
                        gen_push_lit(token->value, INT_LIT);
                        break;
                    case TOKEN_DOUBLE:
                        gen_push_lit(token->value, DOUBLE_LIT);
                        break;
                    case TOKEN_STRING:
                        gen_push_lit(token->value, STRING_LIT);
                        break;
                    default:
                        break;
                }
            }

            if (convert && token->type == TOKEN_INTEGER) {
                gen_expr_conv(IF);
            } else if (convert && token->type == TOKEN_DT_INT) {
                return TYPE_COMPATIBILITY_ERROR;
            }
        } else if (!strcmp(rule, "E+E")) {
            if (conc) {
                gen_string_op('|');
            } else {
                gen_expr_binop('+');
            }
        } else if (!strcmp(rule, "E*E")) {
            gen_expr_binop('*');
        } else if (!strcmp(rule, "E/E")) {
            if (double_div) { // gen depends on division types
                gen_expr_binop('\\');
            } else {
                gen_expr_binop('/');
            }
        } else if (!strcmp(rule, "E-E")) {
            gen_expr_binop('-');
        } else if (!strcmp(rule, "E==E")) {
            gen_cond(EQ);
        } else if (!strcmp(rule, "E!=E")) {
            gen_cond(NEQ);
        } else if (!strcmp(rule, "E<E")) {
            gen_cond(LT);
        } else if (!strcmp(rule, "E<=E")) {
            gen_cond(LTE);
        } else if (!strcmp(rule, "E>E")) {
            gen_cond(GT);
        } else if (!strcmp(rule, "E>=E")) {
            gen_cond(GTE);
        } else if (!strcmp(rule, "E??E")) {
            gen_expr_binop('?');
        }
    }

    return SUCCESS;
}

Error check_comb(stack_char_t* stack, bool only_strings) {
    stack_char_t tmp;
    stack_char_init(&tmp);

    while (!stack_char_empty(stack)) {
        char* rule = stack_char_top(stack);
        if (only_strings) {
            // check if right operators are used with strings
            if (!strcmp(rule, "E/E") || !strcmp(rule, "E-E") || !strcmp(rule, "E*E")) {
                return TYPE_COMPATIBILITY_ERROR;
            }
        } else {
            // check if right operators are used with double and int in one expr
            if (!strcmp(rule, "E<E") || !strcmp(rule, "E<=E") ||
            !strcmp(rule, "E>E") || !strcmp(rule, "E>=E")) {
                return TYPE_COMPATIBILITY_ERROR;
            }
        }

        stack_char_push(&tmp, rule);
        stack_char_pop(stack);
    }

    while (!stack_char_empty(&tmp)) { // push back all rules
        char* rule = stack_char_top(&tmp);
        stack_char_push(stack, rule);
        stack_char_pop(&tmp);
    }

    return SUCCESS;
}

Error check_semantic(Stack* tokenStack, stack_char_t* ruleStack, used_types_t* types, used_types_t* division_types) {
    bool convert = false; // convert int to float
    bool double_div = false; // float division
    bool conc = false; // concatenate strings
    Error err;
    if (types->t_int == true && types->t_double == false && types->t_string == false) {
        err = generate(tokenStack, ruleStack, convert, double_div, conc);
    } else if (types->t_int == false && types->t_double == true && types->t_string == false) {
        double_div = true;
        err = generate(tokenStack, ruleStack, convert, double_div, conc);
    } else if (types->t_int == false && types->t_double == false && types->t_string == true) {
        err = check_comb(ruleStack, true);
        if (err == SUCCESS) {
            conc = true;
            err = generate(tokenStack, ruleStack, convert, double_div, conc);
        } else {
            return err;
        }
    } else if (types->t_int == true && types->t_double == true && types->t_string == false) {
        if (division_types->t_double == true) {
            double_div = true;
            err = check_comb(ruleStack, false);
            if (err == SUCCESS) {
                convert = true;
                err = generate(tokenStack, ruleStack, convert, double_div, conc);
            } else {
                return err;
            }
        } else if (division_types->t_int == true) {
            return TYPE_COMPATIBILITY_ERROR;
        }
    } else if (types->t_string == true && (types->t_double == true || types->t_int == true)) {
        return TYPE_COMPATIBILITY_ERROR;
    }

    return err;
}

Error check_symbol(TokenT* symbol, TokenT** next, Stack* tokenStack, used_types_t* types, symtable_t* symTable, char** symbolRet) {
    if (symbol->type == TOKEN_EOF || symbol->type == TOKEN_DT_DOUBLE || symbol->type == TOKEN_DT_INT || 
    symbol->type == TOKEN_LC_BRACKET || symbol->type == TOKEN_RC_BRACKET ||
    symbol->type == TOKEN_COLON || symbol->type == TOKEN_COMMA || symbol->type == TOKEN_DT_STRING ||
    symbol->type == TOKEN_LET || symbol->type == TOKEN_VAR || symbol->type == TOKEN_RETURN) { // TODO od token return
        *next = symbol;
        *symbolRet = "$"; 
        return SUCCESS;
    }

    if (symbol->type == TOKEN_IDENTIFIER || symbol->type == TOKEN_STRING || symbol->type == TOKEN_INTEGER ||
    symbol->type == TOKEN_DOUBLE) {
        symtable_entry_t* entry;

        switch (symbol->type) {
        case TOKEN_INTEGER:
            types->t_int = true;
            break;
        case TOKEN_STRING:
            types->t_string = true;
            break;
        case TOKEN_DOUBLE:
            types->t_double = true;
            break;
        case TOKEN_IDENTIFIER:
            // token is not literal try to find him in tables
            if (table_search(symTable, symbol->value.str, &entry)) {
                switch (entry->type) {
                    case TOKEN_DT_DOUBLE: 
                        types->t_double = true;
                        symbol->type = TOKEN_DT_DOUBLE;
                        break;
                    case TOKEN_DT_INT:
                        types->t_int = true;
                        symbol->type = TOKEN_DT_INT;
                        break;
                    case TOKEN_DT_STRING:
                        types->t_string = true;
                        symbol->type = TOKEN_DT_STRING;
                        break;
                        // TODO [type]?
                    default: 
                        break;
                }
            } else {
                return UNDEFINED_VARIABLE_ERROR;
            }
            break;
        default:
            break;
        }
        Stack_Push(tokenStack, symbol);
        *symbolRet = "i";
        return SUCCESS;
    }

    *symbolRet = symbol->value.str;
    return SUCCESS;
}

Error check_rule(char* stackRule, stack_char_t* stack, stack_char_t* ruleStack) {
    char expr[NUM_OF_EXPR][MAX_EXP_LEN] = {
    "i", "(E)", "E+E", "E-E", "E*E", "E/E", "E==E", "E!=E", "E<E", "E<=E", "E>E", "E>=E", "E??E"
    };

    for (int i = 0; i < NUM_OF_EXPR; i++) {
        if (!strcmp(stackRule, expr[i])) { // find rule 
            printf("rule: %s\n", expr[i]);
            stack_char_push(stack, "E");
            stack_char_push(ruleStack, stackRule);
            return SUCCESS;
        }
    }

    return SYNTAX_ERROR;
}

Error find_rule(stack_char_t* stack, stack_char_t* ruleStack) {
    stack_char_t tmp;
    stack_char_init(&tmp);

    while (!stack_char_empty(stack)) {
        char* stackTop = stack_char_top(stack);

        if (!strcmp(stackTop, "[")) { // read rule from stack while [ is on top
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
            Error err = check_rule(stackRule, stack, ruleStack);
            return err;
        } else {
            stack_char_push(&tmp, stackTop);
            stack_char_pop(stack);
        }
    }

    return SYNTAX_ERROR;
}

Error type_in_div(TokenT* symbol, symtable_t* symTable, TokenType* type) {
    if (symbol->type == TOKEN_IDENTIFIER || symbol->type == TOKEN_STRING || symbol->type == TOKEN_INTEGER ||
    symbol->type == TOKEN_DOUBLE) {
        symtable_entry_t* entry;

        switch (symbol->type) {
        case TOKEN_INTEGER:
            *type = TOKEN_INTEGER;
            break;
        case TOKEN_STRING:
            *type = TOKEN_STRING;
            break;
        case TOKEN_DOUBLE:
            *type = TOKEN_DOUBLE;
            break;
        case TOKEN_IDENTIFIER:

            if (table_search(symTable, symbol->value.str, &entry)) {
                switch (entry->type) {
                    case TOKEN_DT_DOUBLE: 
                        *type = TOKEN_DT_DOUBLE;
                        break;
                    case TOKEN_DT_INT:
                        *type = TOKEN_DT_INT;
                        break;
                    case TOKEN_DT_STRING:
                        *type = TOKEN_DT_STRING;
                        break;
                        // TODO [type]?
                    default: 
                        break;
                }
            } else {
                return UNDEFINED_VARIABLE_ERROR;
            }
        
        default:    
            break;
        }
    }
    return SUCCESS;
}

Error type_check_div(TokenT* prevprev, TokenT* actual, used_types_t* usedTypes, symtable_t* symTable) {
    TokenType before;
    TokenType after;
    Error err = type_in_div(prevprev, symTable, &before);
    if (err != SUCCESS) {
        return err;
    }
    err = type_in_div(actual, symTable, &after);
    if (err != SUCCESS) {
        return err;
    }

    if ((before == TOKEN_INTEGER || before == TOKEN_DT_INT) && (after == TOKEN_INTEGER || after == TOKEN_DT_INT) && usedTypes->t_double == false) {
        // int/int
        usedTypes->t_int = true;
        return SUCCESS;
    } else if ((before == TOKEN_DOUBLE || before == TOKEN_DT_DOUBLE) && (after == TOKEN_DOUBLE || after == TOKEN_DT_DOUBLE) && usedTypes->t_int == false) {
        // doule/doule
        usedTypes->t_double = true;
        return SUCCESS;
    }

    return TYPE_COMPATIBILITY_ERROR;
}

Error bu_read(TokenT** next, symtable_t* symTable) {
    used_types_t types = {.t_double = false, .t_int = false, .t_string = false};
    used_types_t divTypeResult = {.t_double = false, .t_int = false, .t_string = false};
    bool checkDivision = false; // set flag when division 

    Stack tokenStack;
    Stack_Init(&tokenStack);

    TokenT* prevToken;
    TokenT* prevprevToken;

    stack_char_t stack;
    stack_char_init(&stack);
    stack_char_push(&stack, "$");

    stack_char_t ruleStack;
    stack_char_init(&ruleStack);
    Error err;
    char* symbol;

    char* stackTerminal = stack_char_top(&stack);
    TokenT* token = generate_token();
    err = check_symbol(token, next, &tokenStack, &types, symTable, &symbol);
    if (err != SUCCESS) {
        return err;
    }
    PrecAction action;
    err = get(stackTerminal, symbol, &action);
    if (err != SUCCESS) {
        return err;
    }

    while (strcmp(symbol, "$") != 0 || stack_char_2oftop(&stack) == false) {
        if (err == SUCCESS) {
            switch(action) {
                case PREC_ACTION_SHIFT: 
                    err = stack_insertAfterTerminal(&stack);
                    stack_char_push(&stack, symbol);

                    if ((*next)->type == TOKEN_ZERO) { // token which does not belog to expr was not find 
                        prevToken = token;
                        token = generate_token();
                        if (checkDivision) {
                            err = type_check_div(prevprevToken, token, &divTypeResult, symTable);
                            if (err != SUCCESS) {
                                return TYPE_COMPATIBILITY_ERROR;
                            }
                            checkDivision = false;
                        }
                        if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "/")) {
                            prevprevToken = prevToken;
                            checkDivision = true; // should check division types
                        }
                        err = check_symbol(token, next, &tokenStack, &types, symTable, &symbol);
                        if (err != SUCCESS) {
                            return err;
                        }           
                    }
                    break;
                case PREC_ACTION_REDUCE: // pop from stack and find rule
                    err = find_rule(&stack, &ruleStack);
                    if (err != SUCCESS) {
                        return err;
                    }

                    break;
                case PREC_ACTION_EQ:
                    stack_char_push(&stack, symbol);

                    if ((*next)->type == TOKEN_ZERO) { // token which does not belog to expr was not find 
                        prevToken = token;
                        token = generate_token();
                        if (checkDivision) {
                            err = type_check_div(prevprevToken, token, &divTypeResult, symTable);
                            if (err != SUCCESS) {
                                return TYPE_COMPATIBILITY_ERROR;
                            }
                            checkDivision = false;
                        }
                        if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "/")) {
                            prevprevToken = prevToken;
                            checkDivision = true;
                        }
                        err = check_symbol(token, next, &tokenStack, &types, symTable, &symbol);
                        if (err != SUCCESS) {
                            return err;
                        }
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
        if (err != SUCCESS) {
            return err;
        }
    }
    
    err = check_semantic(&tokenStack, &ruleStack, &types, &divTypeResult);
    return err;
}

int main() {
    TokenT *next = malloc(sizeof(TokenT));
    next->type = TOKEN_ZERO;
    symtable_t symTab;
    table_init(&symTab);
    symtable_entry_t* entry1;
    table_insert(&symTab, "a", &entry1);
    entry1->type = TOKEN_DT_STRING;

    symtable_entry_t* entry2;
    table_insert(&symTab, "c", &entry2);
    entry2->type = TOKEN_DT_STRING;

    Error err = bu_read(&next, &symTab);
    printf("ERROR = %d\n", err);
    return 0;
}