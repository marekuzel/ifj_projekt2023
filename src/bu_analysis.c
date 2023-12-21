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

bool just_type(used_types_t *types, TokenType type) {
    return types->types_used == 1 << (type % 7);
}
bool just_two_types(used_types_t *types, TokenType type1, TokenType type2) {
    return types->types_used == ((1 << (type1 % 7)) | (1 << (type2 % 7)));
}
bool check_type(used_types_t *types, TokenType type) {
    return types->types_used & 1 << (type % 7);
}

void set_type(used_types_t *types, TokenType type) {
    if (type == TOKEN_DT_INT_NIL || type == TOKEN_DT_DOUBLE_NIL || type == TOKEN_DT_STRING_NIL) {
        types->nil_cnts[type]++;
    }
    types->types_used |= 1 << (type % 7);
}

Error generate(Stack* tokenStack, stack_char_t* ruleStack, bool convert, bool conc, symtable_t* symTable, bool assign_to_double) {
    // "i", "(E)", "E+E", "E-E", "E*E", "E/E", "E==E", "E!=E", "E<E", "E<=E", "E>E", "E>=E", "E??E"
    while (!stack_char_empty(ruleStack)) {
        char* rule = stack_bottom_read(ruleStack);
        if(!strcmp(rule, "i")) {
            TokenT *token = stack_read_token_bottom(tokenStack);
            if (is_token_data_type(token)) {
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
            gen_expr_binop(conc ? '|': '+');

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
    int relation_op_counter = 0;

    for (int stack_idx = stack->top; stack_idx >= 0; stack_idx--) {
        char* rule = stack->items[stack_idx];
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
        } else if (!strcmp(rule, "E<E") || !strcmp(rule, "E<=E") || !strcmp(rule, "E>E") || !strcmp(rule, "E>=E")) {
            // check if right operators are used with double and int in one expr
                return TYPE_COMPATIBILITY_ERROR;
        }
    }

    if (if_while && relation_op_counter != 1)
        return TYPE_COMPATIBILITY_ERROR;

    return SUCCESS;
}

Error check_semantic(Stack* tokenStack, stack_char_t* ruleStack, used_types_t* types, used_types_t* division_types, TokenType* exprRetType, symtable_t* symTable, bool if_while, bool assign_to_double) {
    bool convert = false; // convert int to float
    bool conc = false; // concatenate strings
    Error err;

    if (if_while) { // check if true or false will be the result
        CHECK_SUCCES(check_comb,ruleStack, false, false, if_while)
    }
    if (just_type(types,TOKEN_DT_INT)) {

        CHECK_SUCCES(generate,tokenStack, ruleStack, convert, conc, symTable, assign_to_double)
        *exprRetType = assign_to_double ? TOKEN_DT_DOUBLE : TOKEN_DT_INT;

    } else if (just_type(types,TOKEN_DT_DOUBLE)) {

        CHECK_SUCCES(generate,tokenStack, ruleStack, convert, conc, symTable, assign_to_double)
        *exprRetType = TOKEN_DT_DOUBLE;

    } else if (just_type(types,TOKEN_DT_STRING)) {
        CHECK_SUCCES(check_comb,ruleStack, true, false, false)
        conc = true;
        CHECK_SUCCES(generate,tokenStack, ruleStack, convert, conc, symTable, assign_to_double)
        *exprRetType = TOKEN_DT_STRING;

    } else if (just_two_types(types,TOKEN_DT_INT,TOKEN_DT_DOUBLE)) {

        if (check_type(division_types,TOKEN_DT_DOUBLE)|| !check_type(division_types,TOKEN_DT_INT)) {
            CHECK_SUCCES(check_comb,ruleStack, false, false, false)
            convert = true;
            CHECK_SUCCES(generate,tokenStack, ruleStack, convert, conc, symTable, assign_to_double)
            *exprRetType = TOKEN_DT_DOUBLE;

        } else if (check_type(division_types,TOKEN_DT_INT)) {
            return TYPE_COMPATIBILITY_ERROR;
        }
    } else if ((check_type(types, TOKEN_DT_INT) && check_type(types, TOKEN_DT_INT_NIL))
        || (check_type(types, TOKEN_DT_DOUBLE) && (check_type(types,TOKEN_DT_DOUBLE_NIL) || check_type(types,TOKEN_DT_INT_NIL)))
        || (check_type(types, TOKEN_DT_STRING) && (check_type(types,TOKEN_DT_DOUBLE_NIL) || check_type(types,TOKEN_DT_INT_NIL)))) {
        CHECK_SUCCES(check_comb,ruleStack, false, true, false)
        CHECK_SUCCES(generate,tokenStack, ruleStack, convert, conc, symTable, assign_to_double)

        if (check_type(types,TOKEN_DT_INT)) {
            *exprRetType = TOKEN_DT_INT;
        } else if (check_type(types,TOKEN_DT_DOUBLE)) {
            *exprRetType = TOKEN_DT_DOUBLE;
        } else if (check_type(types,TOKEN_DT_STRING)) {
            *exprRetType = TOKEN_DT_STRING;
        }
        
    } else if (check_type(types,TOKEN_NIL)) {
        CHECK_SUCCES(generate,tokenStack, ruleStack, convert, conc, symTable, assign_to_double)
        *exprRetType = TOKEN_NIL;
    } else {
        return TYPE_COMPATIBILITY_ERROR;
    }

    return SUCCESS;
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

    if (symbol->type != TOKEN_IDENTIFIER && symbol->type != TOKEN_STRING && symbol->type != TOKEN_INTEGER
        && symbol->type != TOKEN_DOUBLE && symbol->type != TOKEN_NIL) {

        (*varCounter)--;
        *symbolRet = symbol->value.str;
        return SUCCESS;

    }

    symtable_entry_t* entry;

    (*varCounter)++;
    if (*varCounter > 1) {
        *next = symbol;
        *symbolRet = "$"; 
        return SUCCESS;
    }

    switch (symbol->type) {
        case TOKEN_INTEGER:
            types->types_used |= 1 << TOKEN_DT_INT;
            set_type(types,TOKEN_DT_INT);
            break;
        case TOKEN_STRING:
            set_type(types,TOKEN_DT_STRING);
            break;
        case TOKEN_DOUBLE:
            set_type(types,TOKEN_DT_DOUBLE);
            break;
        case TOKEN_NIL:
            set_type(types,TOKEN_NIL);
            break;
        case TOKEN_IDENTIFIER:
        // token is not literal try to find him in tables
        if (!table_search(symTable, symbol->value.str, &entry)) {
            return UNDEFINED_VARIABLE_ERROR;
        }
        if (entry->type != TOKEN_FUNC) {
            symbol->type = entry->type;
            set_type(types, entry->type);
        }
            break;
        default:
            break;
        }
    Stack_Push(tokenStack, symbol);
    *symbolRet = "i";
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
            return err;
        } else {
            stack_char_push(&tmp, stackTop);
            stack_char_pop(stack);
        }
    }
    return SYNTAX_ERROR;
}

Error type_in_special_expr(TokenT* symbol, symtable_t* symTable, TokenType* type) {
    if (symbol->type != TOKEN_IDENTIFIER) {
        *type = symbol->type;
        return SUCCESS;
    }
    symtable_entry_t* entry;

    if (!table_search(symTable, symbol->value.str, &entry)) {
        return UNDEFINED_VARIABLE_ERROR;
    }
    
    *type = entry->type;
    
    return SUCCESS;

}

Error type_check_div(TokenT* prevprev, TokenT* actual, used_types_t* usedTypes, symtable_t* symTable) {  
    TokenType before;
    TokenType after;

    Error err;
    CHECK_SUCCES(type_in_special_expr,prevprev, symTable, &before)
    CHECK_SUCCES(type_in_special_expr,actual, symTable, &after)

    if ((before == TOKEN_INTEGER || before == TOKEN_DT_INT) && (after == TOKEN_INTEGER || after == TOKEN_DT_INT) && !check_type(usedTypes, TOKEN_DT_DOUBLE)) {
        // int/int
        usedTypes->types_used |= 1 << TOKEN_DT_INT;
        return SUCCESS;
    } else if ((before == TOKEN_DOUBLE || before == TOKEN_DT_DOUBLE) && (after == TOKEN_DOUBLE || after == TOKEN_DT_DOUBLE) && !check_type(usedTypes,TOKEN_DT_INT)) {
        // doule/doule
        usedTypes->types_used |= 1 << TOKEN_DT_DOUBLE;
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

    if (!table_search_global(symTable, token->value.str, &entry)) {
        return UNDEFINED_FUNCTION_ERROR;
    }

    if (entry->type != TOKEN_FUNC) {
        return SYNTAX_ERROR;
    }

    return SUCCESS;
}

Error type_check_2qm(TokenT* prevprev, TokenT* actual, symtable_t* symTable) {
    TokenType before;
    TokenType after;
    Error err;
    CHECK_SUCCES(type_in_special_expr,prevprev, symTable, &before)
    CHECK_SUCCES(type_in_special_expr,actual, symTable, &after)

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

    if (topToken->type != TOKEN_DT_INT_NIL && topToken->type != TOKEN_DT_DOUBLE_NIL && topToken->type != TOKEN_DT_STRING_NIL) {
        return TYPE_COMPATIBILITY_ERROR; 
    }

    types->nil_cnts[topToken->type]--;

    if (types->nil_cnts[topToken->type] == 0) {
        types->types_used ^= topToken->type;
        types->types_used |= topToken->type-1;
    }

    topToken->type--;    

    return SUCCESS;
}

Error bu_read(TokenT** next, Stack* streamTokens, symtable_t* symTable, TokenType* exprRetType, bool if_while, bool assign_to_double) {
    used_types_t types = {0};
    used_types_t divTypeResult = {0};
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
    CHECK_SUCCES(get,stackTerminal, symbol, &action)

    while (strcmp(symbol, "$") != 0 || stack_char_2oftop(&stack) == false) {
        switch(action) {
            case PREC_ACTION_SHIFT: 
                CHECK_SUCCES(stack_insertAfterTerminal,&stack)
                //fall through
            case PREC_ACTION_EQ:
                stack_char_push(&stack, symbol);                    
                if ((*next) != NULL) {  // token which does not belog to expr was not found
                    continue;
                }
                prevToken = token;
                token = stack_read_token_bottom(streamTokens);

                if (token->type == TOKEN_OPERATOR && !strcmp(token->value.str, "!")) { // from [type]? to [type]
                    if (!is_token_data_type(prevToken)) {
                        return TYPE_COMPATIBILITY_ERROR;
                    }
                    CHECK_SUCCES(change_type_withQM,&tokenStack, &types)

                    token = stack_read_token_bottom(streamTokens);
                } 

                if (prevToken->type == TOKEN_IDENTIFIER && token->type == TOKEN_L_BRACKET) { // function call
                    CHECK_SUCCES(isFunc,prevToken, symTable)                        
                    err = deal_with_func(prevToken, symTable, &exprRetType, streamTokens);
                    *next = stack_read_token_bottom(streamTokens);
                    return err;
                }
                if (checkDivision) {
                    CHECK_SUCCES(type_check_div,prevprevToken, token, &divTypeResult, symTable)
                    checkDivision = false;
                }

                if (check2questionmarks) {
                    CHECK_SUCCES(type_check_2qm,prevprevToken, token, symTable)
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
                CHECK_SUCCES(check_symbol,token, next, &tokenStack, &types, symTable, &symbol, &var_count)
                break;
            case PREC_ACTION_REDUCE: // pop from stack and find rule
                CHECK_SUCCES(find_rule,&stack, &ruleStack)
                break;

            case PREC_ACTION_ERROR:
                return SYNTAX_ERROR;
        }

        stack_topTerminal(&stack, &stackTerminal);
        CHECK_SUCCES(get,stackTerminal, symbol, &action)
    }
    
    err = check_semantic(&tokenStack, &ruleStack, &types, &divTypeResult, exprRetType, symTable, if_while, assign_to_double);
    free(tokenStack.array);
    return err;
}
