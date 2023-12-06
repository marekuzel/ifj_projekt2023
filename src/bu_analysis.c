/**
 * Project: Compliler IFJ23 implementation 
 * File: bu_analysis.c
 * 
 * @brief bottom up analysis implementation
 * 
 * @author Tímea Adamčíková xadamc09
*/

#include "utils.h"
#include "prec_table.h"
#include "bu_analysis.h"
#include "code_gen.h"
#include "symtable.h"
#include "errors.h"
#include "scanner.h"
#include <stdio.h>
#include <stdbool.h>

Error generate(Stack* tokenStack, stack_char_t* ruleStack, bool convert, bool conc, symtable_t* symTable, bool assign_to_double) {
    // "i", "(E)", "E+E", "E-E", "E*E", "E/E", "E==E", "E!=E", "E<E", "E<=E", "E>E", "E>=E", "E??E"
    while (!stack_char_empty(ruleStack)) {
        char* rule = stack_bottom_read(ruleStack);
        TokenT* token;
        if(!strcmp(rule, "i")) {
            token = stack_read_token_bottom(tokenStack);
            if (token->type == TOKEN_DT_DOUBLE || token->type == TOKEN_DT_INT || token->type == TOKEN_DT_STRING || 
            token->type == TOKEN_DT_STRING_NIL || token->type == TOKEN_DT_INT_NIL || token->type == TOKEN_DT_DOUBLE_NIL) {
                gen_push_var(token->value.str, is_global(symTable, token->value.str));
            } else {
                gen_push_lit(token->value, token->type);  
            }

            if ((convert || assign_to_double) && token->type == TOKEN_INTEGER) {
                gen_expr_conv(IF);
            } else if ((convert || assign_to_double) && token->type == TOKEN_DT_INT) {
                return TYPE_COMPATIBILITY_ERROR;
            }
        } else if (!strcmp(rule, "E+E")) {
            if (conc) {
                gen_expr_binop('|');
            } else {
                gen_expr_binop('+');
            }
        } else if (!strcmp(rule, "E*E")) {
            gen_expr_binop('*');
        } else if (!strcmp(rule, "E/E")) {
                gen_expr_binop('/');
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
        free(rule);
    }

    return SUCCESS;
}

Error check_comb(stack_char_t* stack, bool only_strings, bool typeNil, bool if_while) {
    // "i", "(E)", "E+E", "E-E", "E*E", "E/E", "E==E", "E!=E", "E<E", "E<=E", "E>E", "E>=E", "E??E"
    stack_char_t tmp;
    stack_char_init(&tmp);
    int relation_op_counter = 0;

    while (!stack_char_empty(stack)) {
        char* rule = stack_char_top(stack);
        if (if_while) { // have to have at least one relation op
            if (!strcmp(rule, "E==E") || !strcmp(rule, "E!=E") || !strcmp(rule, "E<E") || !strcmp(rule, "E<=E") || !strcmp(rule, "E>E") || 
            !strcmp(rule, "E>=E")) {
                relation_op_counter++;
            }
        } else if (typeNil) { // cant use this rules with nil
            if (!strcmp(rule, "E/E") || !strcmp(rule, "E-E") || !strcmp(rule, "E*E") || !strcmp(rule, "E<E") || !strcmp(rule, "E<=E") || 
            !strcmp(rule, "E>E") || !strcmp(rule, "E>=E")) {
                return TYPE_COMPATIBILITY_ERROR;
            }
        } else if (only_strings) {
            // check if right operators are used with strings
            if (!strcmp(rule, "E/E") || !strcmp(rule, "E-E") || !strcmp(rule, "E*E")) {
                return TYPE_COMPATIBILITY_ERROR;
            }
        } else if (only_strings == false && typeNil == false && if_while == false) {
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


    if (if_while && relation_op_counter == 1) {
        return SUCCESS;
    } else if (if_while && relation_op_counter != 1){
        return TYPE_COMPATIBILITY_ERROR;
    }

    return SUCCESS;
}

Error check_semantic(Stack* tokenStack, stack_char_t* ruleStack, used_types_t* types, used_types_t* division_types, TokenType** exprRetType, symtable_t* symTable, bool if_while, bool assign_to_double) {
    bool convert = false; // convert int to float
    bool conc = false; // concatenate strings
    Error err = SUCCESS;

    if (if_while) { // check if true or false will be the result
        err = check_comb(ruleStack, false, false, if_while);
        if (err != SUCCESS) {
            return err;
        }
    }
    if (types->t_int == true && types->t_double == false && types->t_string == false && types->t_int_nil == false && types->t_double_nil == false && types->t_string_nil == false && types->t_nil == false) {
        err = generate(tokenStack, ruleStack, convert, conc, symTable, assign_to_double);
        if (assign_to_double) {
            **exprRetType = TOKEN_DT_DOUBLE;
        } else {
            **exprRetType = TOKEN_DT_INT;
        }
    } else if (types->t_int == false && types->t_double == true && types->t_string == false && types->t_int_nil == false && types->t_double_nil == false && types->t_string_nil == false && types->t_nil == false) {
        err = generate(tokenStack, ruleStack, convert, conc, symTable, assign_to_double);
        **exprRetType = TOKEN_DT_DOUBLE;
    } else if (types->t_int == false && types->t_double == false && types->t_string == true && types->t_int_nil == false && types->t_double_nil == false && types->t_string_nil == false && types->t_nil == false) {
        err = check_comb(ruleStack, true, false, false);
        if (err == SUCCESS) {
            conc = true;
            err = generate(tokenStack, ruleStack, convert, conc, symTable, assign_to_double);
            **exprRetType = TOKEN_DT_STRING;
        } else {
            return err;
        }
    } else if (types->t_int == true && types->t_double == true && types->t_string == false && types->t_int_nil == false && types->t_double_nil == false && types->t_string_nil == false && types->t_nil == false) {
        if (division_types->t_double == true || (division_types->t_double == false && division_types->t_int == false)) {
            err = check_comb(ruleStack, false, false, false);
            if (err == SUCCESS) {
                convert = true;
                err = generate(tokenStack, ruleStack, convert, conc, symTable, assign_to_double);
                **exprRetType = TOKEN_DT_DOUBLE;
            } else {
                return err;
            }
        } else if (division_types->t_int == true) {
            return TYPE_COMPATIBILITY_ERROR;
        }
    } else if ((types->t_int == true && (types->t_int_nil == true || types->int_nil == true)) || (types->t_double == true && (types->double_nil == true || types->int_nil == true)) || 
    (types->t_string == true && (types->t_string_nil == true || types->int_nil == true))) {
        err = check_comb(ruleStack, false, true, false);
        if (err == SUCCESS) {
            err = generate(tokenStack, ruleStack, convert, conc, symTable, assign_to_double);
            if (types->t_int == true) {
                **exprRetType = TOKEN_DT_INT;
            } else if (types->t_double == true) {
                **exprRetType = TOKEN_DT_DOUBLE;
            } else if (types->t_string == true) {
                **exprRetType = TOKEN_DT_STRING;
            }
        } else {
            return err;
        }

    } else if (types->t_string == true && (types->t_double == true || types->t_int == true)) {
        return TYPE_COMPATIBILITY_ERROR;
    } else if (types->t_nil == true) {
        generate(tokenStack, ruleStack, convert, conc, symTable, assign_to_double);
        **exprRetType = TOKEN_NIL;
    } else {
        return TYPE_COMPATIBILITY_ERROR;
    }

    return err;
}

Error check_symbol(TokenT* symbol, TokenT** next, Stack* tokenStack, used_types_t* types, symtable_t* symTable, char** symbolRet, int* varCounter) {
    if (symbol->type == TOKEN_EOF || symbol->type == TOKEN_DT_DOUBLE || symbol->type == TOKEN_DT_INT || 
    symbol->type == TOKEN_LC_BRACKET || symbol->type == TOKEN_RC_BRACKET ||
    symbol->type == TOKEN_COLON || symbol->type == TOKEN_COMMA || symbol->type == TOKEN_DT_STRING ||
    symbol->type == TOKEN_LET || symbol->type == TOKEN_VAR || symbol->type == TOKEN_RETURN || symbol->type == TOKEN_IF ||
    symbol->type == TOKEN_ELSE || symbol->type == TOKEN_WHILE || symbol->type == TOKEN_FUNC || 
    symbol->type == TOKEN_ASSIGN || symbol->type == TOKEN_ARROW) { 
        *next = symbol;
        *symbolRet = "$"; 
        return SUCCESS;
    }

    if (symbol->type == TOKEN_L_BRACKET || symbol->type == TOKEN_R_BRACKET) {
        *symbolRet = symbol->value.str;
        return SUCCESS;
    }

    if (symbol->type == TOKEN_IDENTIFIER || symbol->type == TOKEN_STRING || symbol->type == TOKEN_INTEGER ||
    symbol->type == TOKEN_DOUBLE || symbol->type == TOKEN_NIL) {
        symtable_entry_t* entry;
        (*varCounter)++;
        if (*varCounter > 1) {
            *next = symbol;
            *symbolRet = "$"; 
            return SUCCESS;
        }

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
        case TOKEN_NIL:
            types->t_nil = true;
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
                    case TOKEN_DT_DOUBLE_NIL: 
                        types->t_double_nil = true;
                        types->double_nil++;
                        symbol->type = TOKEN_DT_DOUBLE_NIL;
                        break;
                    case TOKEN_DT_INT_NIL:
                        types->t_int_nil = true;
                        types->int_nil++;
                        symbol->type = TOKEN_DT_INT_NIL;
                        break;
                    case TOKEN_DT_STRING_NIL:
                        types->t_string_nil = true;
                        types->string_nil++;
                        symbol->type = TOKEN_DT_STRING_NIL;
                        break;
                    // case TOKEN_FUNC:
                    //     *next = symbol;
                    //     *symbolRet = "$"; 
                    //     return SUCCESS;
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

    (*varCounter)--;
    *symbolRet = symbol->value.str;
    return SUCCESS;
}

Error check_rule(char* stackRule, stack_char_t* stack, stack_char_t* ruleStack) {
    char expr[NUM_OF_EXPR][MAX_EXP_LEN] = {
    "i", "(E)", "E+E", "E-E", "E*E", "E/E", "E==E", "E!=E", "E<E", "E<=E", "E>E", "E>=E", "E??E"
    };

    for (int i = 0; i < NUM_OF_EXPR; i++) {
        if (!strcmp(stackRule, expr[i])) { // find rule 
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
            // free(stackRule);
            return err;
        } else {
            stack_char_push(&tmp, stackTop);
            stack_char_pop(stack);
        }
    }

    return SYNTAX_ERROR;
}

Error type_in_special_expr(TokenT* symbol, symtable_t* symTable, TokenType* type) {
    if (symbol->type == TOKEN_IDENTIFIER) {
        symtable_entry_t* entry;

        if (table_search(symTable, symbol->value.str, &entry)) {
            switch (entry->type) {
                case TOKEN_DT_DOUBLE: 
                    *type = TOKEN_DT_DOUBLE;
                    return SUCCESS;
                case TOKEN_DT_INT:
                    *type = TOKEN_DT_INT;
                    return SUCCESS;
                case TOKEN_DT_STRING:
                    *type = TOKEN_DT_STRING;
                    return SUCCESS;
                case TOKEN_DT_DOUBLE_NIL:
                    *type = TOKEN_DT_DOUBLE_NIL;
                    return SUCCESS; 
                case TOKEN_DT_INT_NIL: 
                    *type = TOKEN_DT_INT_NIL;
                    return SUCCESS; 
                case TOKEN_DT_STRING_NIL:
                    *type = TOKEN_DT_STRING_NIL;
                    return SUCCESS; 
                default: 
                    break;
            }
        } else {
            return UNDEFINED_VARIABLE_ERROR;
        }
    }
    *type = symbol->type;
    return SUCCESS;
}

Error type_check_div(TokenT* prevprev, TokenT* actual, used_types_t* usedTypes, symtable_t* symTable) {  
    TokenType before;
    TokenType after;

    Error err = type_in_special_expr(prevprev, symTable, &before);
    if (err != SUCCESS) {
        return err;
    }
    err = type_in_special_expr(actual, symTable, &after);
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

Error deal_with_func(TokenT* token, symtable_t* symTable, TokenType** resType, Stack* streamTokens) {
    symtable_entry_t* entry;
    table_search_global(symTable, token->value.str, &entry);
    char *func_name = token->value.str;
    int param_idx = 0;
    while (entry->params[param_idx] != NULL) {
        int tmp_idx = param_idx + 1;

        bool comma = true;
        if (entry->params[tmp_idx] == NULL) {
            comma = false;
        }

        if (!strcmp(entry->params[param_idx]->name , "_")) { // [expr]
            token = stack_read_token_bottom(streamTokens);
            if (token->type == TOKEN_IDENTIFIER) {
                symtable_entry_t* paramIdent;

                if (table_search(symTable, token->value.str, &paramIdent)) { // find variable in symtable
                    if (paramIdent->type == TOKEN_ZERO) {
                        symTable->top_idx--;
                        table_search(symTable, token->value.str, &paramIdent);
                        symTable->top_idx++;
                    }
                    if (paramIdent->type != entry->params[param_idx]->type) { // parameters in func definition and func call must have same type
                        return WRONG_NUM_TYPE_ERROR;
                    }

                    param_value_init(symTable, entry->params[param_idx], token->value, token->type);
                } else {
                    return UNDEFINED_VARIABLE_ERROR;
                }
            } else {
                if ((token->type == TOKEN_INTEGER && entry->params[param_idx]->type == TOKEN_DT_INT) ||
                        (token->type == TOKEN_DOUBLE && entry->params[param_idx]->type == TOKEN_DT_DOUBLE) ||
                        (token->type == TOKEN_STRING && entry->params[param_idx]->type == TOKEN_DT_STRING)) {
                    param_value_init(symTable, entry->params[param_idx], token->value, token->type);
                } else {
                    return WRONG_NUM_TYPE_ERROR;
                }
            }

            if (comma) { // func has more params 
                token = stack_read_token_bottom(streamTokens);
                if (token->type != TOKEN_COMMA) {
                    return SYNTAX_ERROR;
                }
            }
        } else { // [name] : [expr]
            token = stack_read_token_bottom(streamTokens);
            if (token->type == TOKEN_IDENTIFIER && !strcmp(token->value.str, entry->params[param_idx]->name)) {

                token = stack_read_token_bottom(streamTokens);
                if (token->type != TOKEN_COLON) {
                    return SYNTAX_ERROR;
                }

                token = stack_read_token_bottom(streamTokens);
                if (token->type == TOKEN_IDENTIFIER) {
                    symtable_entry_t* paramIdent;

                    if (table_search_global(symTable, token->value.str, &paramIdent)) { // find variable in symtable
                        if (paramIdent->type != entry->params[param_idx]->type) { // parameters in func definition and func call must have same type
                            return WRONG_NUM_TYPE_ERROR;
                        }
                    param_value_init(symTable, entry->params[param_idx], token->value, token->type);
                    } else {
                        return UNDEFINED_VARIABLE_ERROR;
                    }
                } else {
                    if ((token->type == TOKEN_INTEGER && entry->params[param_idx]->type == TOKEN_DT_INT) ||
                            (token->type == TOKEN_DOUBLE && entry->params[param_idx]->type == TOKEN_DT_DOUBLE) ||
                            (token->type == TOKEN_STRING && entry->params[param_idx]->type == TOKEN_DT_STRING)) {
                        param_value_init(symTable, entry->params[param_idx], token->value, token->type);
                    } else {
                        return WRONG_NUM_TYPE_ERROR;
                    }
                }

                if (comma) { // func has more params 
                    token = stack_read_token_bottom(streamTokens);
                    if (token->type != TOKEN_COMMA) {
                        return SYNTAX_ERROR;
                    }
                }
            } else {
                return WRONG_NUM_TYPE_ERROR;
            }
        }

        param_idx++;
    }

    token = stack_read_token_bottom(streamTokens);
    if (token->type == TOKEN_R_BRACKET) {
        **resType = entry->return_type;
        table_add_scope(symTable);
        add_params_to_scope(symTable,entry);
        gen_func_call(func_name,entry);
        table_remove_scope(symTable);
        return SUCCESS;
    }

    return WRONG_NUM_TYPE_ERROR;
}

Error isFunc(TokenT* token, symtable_t* symTable) {
    symtable_entry_t* entry;

    if (table_search_global(symTable, token->value.str, &entry)) {
        if (entry->type == TOKEN_FUNC) {
            return SUCCESS;
        } else {
            return SYNTAX_ERROR;
        }
    }

    return UNDEFINED_FUNCTION_ERROR;
}

Error type_check_2qm(TokenT* prevprev, TokenT* actual, symtable_t* symTable) {
    TokenType before;
    TokenType after;
    Error err = type_in_special_expr(prevprev, symTable, &before);
    if (err != SUCCESS) {
        return err;
    }
    err = type_in_special_expr(actual, symTable, &after);
    if (err != SUCCESS) {
        return err;
    }

    if (before == TOKEN_NIL && (after == TOKEN_INTEGER || after == TOKEN_DOUBLE || after == TOKEN_STRING || after == TOKEN_DT_DOUBLE ||
        after == TOKEN_DT_INT || after == TOKEN_DT_STRING)) {
        return SUCCESS;
    } else if (before == TOKEN_DT_INT_NIL && (after == TOKEN_INTEGER || after == TOKEN_DT_INT)) {
        return SUCCESS;
    } else if (before == TOKEN_DT_DOUBLE_NIL && (after == TOKEN_DOUBLE || after == TOKEN_DT_DOUBLE)) {
        return SUCCESS;
    } else if (before == TOKEN_DT_STRING_NIL && (after == TOKEN_STRING || after == TOKEN_DT_STRING)) {
        return SUCCESS;
    }
    return TYPE_COMPATIBILITY_ERROR;
}

Error change_type_withQM(Stack* tokenStack, used_types_t* types) {
    TokenT* topToken;
    Stack_Top(tokenStack, &topToken); // change type of top token
    Stack_Pop(tokenStack);
    switch(topToken->type) {
        case TOKEN_DT_INT_NIL:
            types->int_nil--;
            if (types->int_nil == 0) {
                types->t_int_nil = false;
                types->t_int = true;
            }
            topToken->type = TOKEN_DT_INT;
            break;
        case TOKEN_DT_DOUBLE_NIL:
            types->double_nil--;
            if (types->double_nil == 0) {
                types->t_double_nil = false;
                types->t_double = true;
            }
            topToken->type = TOKEN_DT_DOUBLE;
            break;
        case TOKEN_DT_STRING_NIL:
            types->string_nil--;
            if (types->string_nil == 0) {
                types->t_string_nil = false;
                types->t_string = true;
            }
            topToken->type = TOKEN_DT_STRING;
            break;
        default:
            return TYPE_COMPATIBILITY_ERROR;
    } 
    Stack_Push(tokenStack, topToken);
    return SUCCESS;
}

bool is_data_type(TokenT* token) {
    switch (token->type) {
        case TOKEN_DT_INT: case TOKEN_DT_INT_NIL: case TOKEN_DT_DOUBLE: case TOKEN_DT_DOUBLE_NIL: case TOKEN_DT_STRING: case TOKEN_DT_STRING_NIL:
            return true;
        default:
            return false;
    }
}

Error bu_read(TokenT** next, Stack* streamTokens, symtable_t* symTable, TokenType* exprRetType, bool if_while, bool assign_to_double) {
    used_types_t types = {.t_double = false, .t_int = false, .t_string = false, .t_int_nil = false, .int_nil = 0,
                            .t_double_nil = false, .double_nil = 0, .t_string_nil = false, .string_nil = 0};
    used_types_t divTypeResult = {.t_double = false, .t_int = false, .t_string = false, .t_int_nil = false, .int_nil = 0,
                            .t_double_nil = false, .double_nil = 0, .t_string_nil = false, .string_nil = 0};
    bool checkDivision = false; // set flag when division
    bool check2questionmarks = false; // set flag when ??
    Error err;
    int var_count = 0;
    *exprRetType = TOKEN_ZERO;

    TokenT* token = stack_read_token_bottom(streamTokens);

    Stack tokenStack;
    Stack_Init(&tokenStack);

    TokenT* prevToken;
    TokenT* prevprevToken;

    stack_char_t stack;
    stack_char_init(&stack);
    stack_char_push(&stack, "$");

    stack_char_t ruleStack;
    stack_char_init(&ruleStack);
    char* symbol;

    char* stackTerminal = stack_char_top(&stack);
    err = check_symbol(token, next, &tokenStack, &types, symTable, &symbol, &var_count);
    if (err != SUCCESS) {
        if (err == UNDEFINED_VARIABLE_ERROR) {
            token = stack_read_token_bottom(streamTokens);
            if (token->type == TOKEN_L_BRACKET) {
                return UNDEFINED_FUNCTION_ERROR;
            }
        }
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
                    if ((*next) == NULL) { // token which does not belog to expr was not found
                        prevToken = token;
                        token = stack_read_token_bottom(streamTokens);

                        if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "!")) { // from [type]? to [type]
                            if (!is_data_type(prevToken)) {
                                return TYPE_COMPATIBILITY_ERROR;
                            }
                            err = change_type_withQM(&tokenStack, &types);
                            if (err != SUCCESS) {
                                return err;
                            }
                            token = stack_read_token_bottom(streamTokens);
                        } 

                        if (prevToken->type == TOKEN_IDENTIFIER && token->type == TOKEN_L_BRACKET) { // function call
                            err = isFunc(prevToken, symTable);
                            if (err == SUCCESS) {
                                err = deal_with_func(prevToken, symTable, &exprRetType, streamTokens);
                                *next = stack_read_token_bottom(streamTokens);
                                return err;
                            } else {
                                return err;
                            }
                        }
                        if (checkDivision) {
                            err = type_check_div(prevprevToken, token, &divTypeResult, symTable);
                            if (err != SUCCESS) {
                                return err;
                            }
                            checkDivision = false;
                        }

                        if (check2questionmarks) {
                            err = type_check_2qm(prevprevToken, token, symTable);
                            if (err != SUCCESS) {
                                return err;
                            }
                            check2questionmarks = false;
                        }

                        if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "??")) {
                            prevprevToken = prevToken;
                            check2questionmarks = true; // should check division types
                        }

                        if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "/")) {
                            prevprevToken = prevToken;
                            checkDivision = true; // should check division types
                        }
                        err = check_symbol(token, next, &tokenStack, &types, symTable, &symbol, &var_count);
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

                    if ((*next) == NULL) { // token which does not belog to expr was not found  
                        prevToken = token;
                        token = stack_read_token_bottom(streamTokens);

                        if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "!")) { // from [type]? to [type]
                            if (!is_data_type(prevToken)) {
                                return SYNTAX_ERROR;
                            }
                            err = change_type_withQM(&tokenStack, &types);
                            if (err != SUCCESS) {
                                return err;
                            }
                            token = stack_read_token_bottom(streamTokens);
                        } 

                        if (prevToken->type == TOKEN_STRING && token->type == TOKEN_L_BRACKET) { // function call
                            err = isFunc(prevToken, symTable);
                            if (err == SUCCESS) {
                                err = deal_with_func(prevToken, symTable, &exprRetType, streamTokens);
                                *next = stack_read_token_bottom(streamTokens);
                                return err;
                            } else {
                                return err;
                            }
                        }
                        if (checkDivision) {
                            err = type_check_div(prevprevToken, token, &divTypeResult, symTable);
                            if (err != SUCCESS) {
                                return err;
                            }
                            checkDivision = false;
                        }

                        if (check2questionmarks) {
                            err = type_check_2qm(prevprevToken, token, symTable);
                            if (err != SUCCESS) {
                                return err;
                            }
                            check2questionmarks = false;
                        }
                        
                        if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "??")) {
                            prevprevToken = prevToken;
                            check2questionmarks = true; // should check division types
                        }

                        if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "/")) {
                            prevprevToken = prevToken;
                            checkDivision = true;
                        }
                        err = check_symbol(token, next, &tokenStack, &types, symTable, &symbol, &var_count);
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
    
    err = check_semantic(&tokenStack, &ruleStack, &types, &divTypeResult, &exprRetType, symTable, if_while, assign_to_double);
    free(tokenStack.array);
    return err;
}
