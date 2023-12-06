/**
 * Project: Compliler IFJ23 implementation 
 * File: syntax_rules.c
 * 
 * @brief implementation of syntax rules and generation
 * 
 * @authors Marek Kužel xkuzel11 
 *          Tomáš Zgút xzgutt00
 *          Tímea Adamčíková xadamc09 
*/

#include "syntax_rules.h"
#include "parser.h"
#include "code_gen.h"

#define TEST_PARSER
#ifdef TEST_PARSER
#define PRINT_RULE(rule) printf("# rule %s\n", #rule);
#else
#define PRINT_RULE(rule)
#endif

Error RuleErr = SUCCESS; //global value for success of the rules

Error parser_rule_id(Parser_t *parser){
    //id ->id
    PRINT_RULE(id);
    // print_token(parser->token_current);
    CHECK_TOKEN_TYPE(parser, TOKEN_IDENTIFIER);
    parser->current_id = parser->token_current->value.str;
    return SUCCESS;
}

Error parser_rule_funcID(Parser_t *parser){
    //funcId -> id
    PRINT_RULE(funcID);
    // print_token(parser->token_current);
    CHECK_TOKEN_TYPE(parser, TOKEN_IDENTIFIER);
    parser->current_function = parser->token_current->value.str;
    return SUCCESS;
}

Error parser_rule_stmt(Parser_t *parser){
    //stmt -> let <id> <stmt_assign>
    PRINT_RULE(stmt);
    // print_token(parser->token_current);
    if (parser->token_current->type == TOKEN_LET){
        PRINT_RULE(Let);
        GET_NEXT_AND_CALL_RULE(parser, id);

        if (table_search_local(parser->symtable,parser->current_id)) {
            return UNDEFINED_FUNCTION_ERROR;
        }

        table_insert(parser->symtable,parser->current_id,&(parser->current_entry));
        parser->current_entry->declared = true;
        parser->current_entry->constant = true;
        gen_def_var(parser->current_id,is_global(parser->symtable,parser->current_id),parser->current_entry->type);
        GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
        goto success;
    }
    //stmt -> var <id> <stmt_assign>
    else if (parser->token_current->type == TOKEN_VAR){
        PRINT_RULE(var);
        GET_NEXT_AND_CALL_RULE(parser, id);
        //add variable to symtable and assign that its declared
        if (table_search_local(parser->symtable,parser->current_id)) {
                return UNDEFINED_FUNCTION_ERROR;
        }
        table_insert(parser->symtable, parser->current_id, &(parser->current_entry));
        parser->current_entry->declared = true;
        parser->current_entry->constant = false;
        gen_def_var(parser->current_id,is_global(parser->symtable,parser->current_id),parser->current_entry->type);
        GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
        goto success;
    }
    //stmt -> 
     // | if <expr> { <stmtSeq> }
    //  | if let [id] [stmtAssign] [stmtSeq] [stmt_else]
    else if (parser->token_current->type == TOKEN_IF){ //add local symbol table
        PRINT_RULE(ifS);
        parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_LET){
            GET_NEXT_AND_CALL_RULE(parser, id);
            symtable_entry_t* entry;
            if (table_search(parser->symtable, parser->current_id, &entry)) {
                switch (entry->type) {
                    case TOKEN_DT_DOUBLE_NIL: case TOKEN_DT_INT_NIL: case TOKEN_DT_STRING_NIL:
                        if (entry->constant != true) {
                            return TYPE_COMPATIBILITY_ERROR; // TODO check me
                        } 
                        break;
                    default: 
                        return TYPE_COMPATIBILITY_ERROR; // TODO check me
                }
            } else {
                return UNDEFINED_VARIABLE_ERROR;
            }
            int cond_label = get_cond_label();
            litValue value = {.d = 0.0};
            gen_push_var(parser->current_id,is_global(parser->symtable,parser->current_id));
            gen_push_lit(value,TOKEN_NIL);
            gen_cond(NEQ);
            gen_cnd_jump(ELSE_L,cond_label);
            table_add_scope(parser->symtable);
            entry->type--; //changeing datatype to without ?
            gen_local_scope(parser->symtable);
            GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
            GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
            gen_jmp(IF_END_L,cond_label);
            gen_cond_else_label(cond_label);
            entry->type++; //changing datatype back to with ?
            parser_getNewToken(parser);
            if (parser->token_current->type == TOKEN_ELSE){
                RuleErr = parser_rule_elseF(parser);
                RETURN_ERROR;
            }
            else {
                parser_stashExtraToken(parser, parser->token_current);
            }
            gen_end_label(IF_L,cond_label);
            gen_drop_local_scope(parser->symtable);
            table_remove_scope(parser->symtable);
            // GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_RC_BRACKET);
            // GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
        }
        else{
            parser->if_while = true;
            int cnd_label = get_cond_label();
            parser->stack->bottomIndex--;
            RuleErr = parser_rule_expr(parser); //with note that expr will handle the brackets

            RETURN_ERROR;
            GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
            table_add_scope(parser->symtable);
            gen_local_scope(parser->symtable);
            gen_cnd_jump(ELSE_L,cnd_label);

            GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
            gen_drop_local_scope(parser->symtable);
            table_remove_scope(parser->symtable);
            gen_jmp(IF_END_L,cnd_label);
            gen_cond_else_label(cnd_label);
            GET_NEXT_AND_CALL_RULE(parser, elseF);
            gen_drop_local_scope(parser->symtable);
            gen_end_label(IF_L,cnd_label);
        }
        goto success;
    }
    //stmt -> while [expr] { [stmt_seqFunc] }
    else if (parser->token_current->type == TOKEN_WHILE){
        PRINT_RULE (whileS);
        parser->if_while = true;
        int loop_label = get_loop_label();
        table_add_scope(parser->symtable);
        gen_local_scope(parser->symtable);
        gen_loop_label(loop_label);
        RuleErr = parser_rule_expr(parser);
        RETURN_ERROR;
        gen_cnd_jump(LOOP_END_L,loop_label);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);

        gen_jmp(LOOP_L,loop_label);
        gen_end_label(LOOP_L,loop_label);
        gen_drop_local_scope(parser->symtable);
        table_remove_scope(parser->symtable);

        goto success;
    }
    //stmt -> <defFunc>
    else if (parser->token_current->type == TOKEN_FUNC){
        int cont_label = get_cont_label();
        jump_cont_label(cont_label);
        //table_add_scope(parser->symtable);
        GET_NEXT_AND_CALL_RULE(parser, defFunc);
        //table_drop_scope_symtable
        gen_cont_label(cont_label);
        goto success;
    }
    //stmt -> [id] -> = [expr]
    else if (parser->token_current->type == TOKEN_IDENTIFIER){
        symtable_entry_t *tmp;
        if (table_search_global(parser->symtable,parser->token_current->value.str,&tmp) && tmp->type == TOKEN_FUNC) {
            parser->current_function = parser->token_current->value.str;
            PRINT_RULE(callFunc)
            RuleErr = parser_rule_callFunc(parser);
            RETURN_ERROR;
            goto success;

        } else {
            PRINT_RULE(stmtID);
            parser->current_id = parser->token_current->value.str;
            GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_ASSIGN);
            parser->assign = true;
            RuleErr = parser_rule_expr(parser);
            RETURN_ERROR;
            gen_assignment(parser->current_id,is_global(parser->symtable,parser->current_id));
            goto success;
        }
    }
    else{
        PRINT_RULE(SYNTAX_ERROR);
        return SYNTAX_ERROR;
    }
    success:
        return SUCCESS;
}

Error parser_rule_stmtAssign(Parser_t *parser){
    PRINT_RULE(stmtAssign);
    //stmt_assign -> = <expr>
    if (parser->token_current->type == TOKEN_ASSIGN){
        parser->find_id_type = true;
        RuleErr = parser_rule_expr(parser);
        RETURN_ERROR;
        gen_assignment(parser->current_id,is_global(parser->symtable,parser->current_id));
        if (parser->current_entry->defined == false) {
            parser->current_entry->defined = true;
        } else {
            parser->current_entry->modified = true;
        }
        goto success;
    }
    //stmt_assign -> : <type>
    //stmt_assign -> : <type> = [expr]
    else if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
        parser->current_entry->type = parser->token_current->type;
        parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_ASSIGN){
            parser->current_entry->defined = true;
            RuleErr = parser_rule_expr(parser);
            RETURN_ERROR;
            gen_assignment(parser->current_id,is_global(parser->symtable,parser->current_id));
            goto success;
        }
        else{
            parser_stashExtraToken(parser, parser->token_current);
            goto success;
        }
    }
    else{
        return SYNTAX_ERROR;
    }
    success:
        return SUCCESS;
}

Error parser_rule_paramsCall (Parser_t *parser){
    PRINT_RULE(paramsCall);
    //     [parametersCall] →
    //    | [name] : [expr] [parameters_seqCall]
    //    | [expr] [parameters_seqCall]
    //    | )
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
    parser_getNewToken(parser);
    if (parser->token_current->type == TOKEN_IDENTIFIER){
        parser->current_function = parser->token_current->value.str;
        GET_NEXT_AND_CALL_RULE(parser, id); //name is id from syntax pow

        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        RuleErr = parser_rule_expr(parser);
        RETURN_ERROR;
        
        GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
        goto success;
    }
    else if (parser->token_current->type == TOKEN_L_BRACKET){
        goto success;
    }
    else {
        parser->stack->bottomIndex--;
        RuleErr =parser_rule_expr(parser);
        RETURN_ERROR;
        GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
    }
    success:
        return SUCCESS;
}

Error parser_rule_paramsCallSeq (Parser_t * parser){
    // [parameters_seqCall] →
    //    | , [name] : [expr] [parameters_seq]
    //    | , [expr]
    //    | // empty
    if (parser->token_current->type == TOKEN_COMMA){

        parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_IDENTIFIER){
            GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
            RuleErr = parser_rule_expr(parser);
            RETURN_ERROR;
            GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
            return SUCCESS;
        }
        else{
            parser->stack->bottomIndex--;
            RuleErr =parser_rule_expr(parser);
            RETURN_ERROR;
            GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
            return SUCCESS;
        }
    }
    else if (parser->token_current->type == TOKEN_R_BRACKET){
        table_remove_scope(parser->symtable);
        return SUCCESS;
    }
    return SYNTAX_ERROR;
}

Error parser_rule_elseF(Parser_t *parser){
    //else -> else { <stmtSeq>
    PRINT_RULE(elsef);
    if (parser->token_current->type == TOKEN_ELSE){
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);

        table_add_scope(parser->symtable);
        gen_local_scope(parser->symtable);

        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
    }
    //otherwise empty
    return SUCCESS;
}

Error parser_rule_defFunc(Parser_t *parser){
    PRINT_RULE(defFunc)
    //func [funcId] ([parameters]) [func_ret]
    // GET_NEXT_AND_CALL_RULE(parser, funcID);
    RuleErr=  parser_rule_funcID(parser);
    RETURN_ERROR;
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);
    GET_NEXT_AND_CALL_RULE(parser, paramsDef);
    gen_func_def(parser->current_function);
    GET_NEXT_AND_CALL_RULE(parser, funcRet);
    gen_func_return();
    return SUCCESS;
}

Error parser_rule_funcRet(Parser_t *parser){
    PRINT_RULE(funcRet);
    if (parser->token_current->type == TOKEN_ARROW){
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, stmtSeqRet);
        return SUCCESS;
    }   
    else if (parser->token_current->type == TOKEN_LC_BRACKET){
        GET_NEXT_AND_CALL_RULE(parser, stmtVoidSeqRet);
        return SUCCESS;
    }
    return SYNTAX_ERROR;
}

Error parser_rule_stmtSeqRet(Parser_t *parser){
    PRINT_RULE(stmtSeqRet)
    // [stmt_seqFunc] → 
    //    | [stmt]
    //    | return [expr]
    while (1){
        if (parser->token_current->type == TOKEN_RETURN){
            parser->return_in_func = true;
            RuleErr = parser_rule_expr(parser);
            RETURN_ERROR;
            parser_getNewToken(parser);
            continue;
        }
        else if (parser->token_current->type == TOKEN_RC_BRACKET){
            return SUCCESS;
        }
        else{
            RuleErr= parser_rule_stmt(parser);
            RETURN_ERROR;
        }
    return SUCCESS;
    }
}

Error parser_rule_stmtVoidSeqRet(Parser_t *parser){
    PRINT_RULE(parser_rule_stmtVoidSeqRet);
    // [stmt_seqVoidSeqRet] → 
    //    | [stmt]
    //    | return
    while (parser->token_current->type != TOKEN_RC_BRACKET){
        // print_token(parser->token_current);
        if (parser->token_current->type == TOKEN_RETURN){
            gen_func_return();
            parser_getNewToken(parser);
            continue;
        }
        else{
            RuleErr = parser_rule_stmt(parser);
            RETURN_ERROR;
        }
        parser_getNewToken(parser);
    }
    return SUCCESS;
}

Error func_write_call(Parser_t *parser, symtable_entry_t* entry) { // write(term1, term3, .. , termn)
    PRINT_RULE(func_write_call);
    parser_getNewToken(parser);
    int param_idx = 0;
    while (parser->token_current->type != TOKEN_R_BRACKET) {  
        if (parser->token_current->type == TOKEN_IDENTIFIER) {
            symtable_entry_t* paramIdent;
            if (table_search(parser->symtable, parser->token_current->value.str, &paramIdent)) { // find variable in symtable
                if (paramIdent->type != TOKEN_FUNC) {
                    param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, parser->token_current->type);
                    entry->params[0]->type = TOKEN_VAR;
                } else {
                    return WRONG_NUM_TYPE_ERROR;
                }
                if (paramIdent->declared == false) { // TODO check me
                    return UNSPECIFIED_TYPE_ERROR;
                }
            } else {
                return UNDEFINED_VARIABLE_ERROR;
            }
        } else if (parser->token_current->type == TOKEN_INTEGER || parser->token_current->type == TOKEN_STRING || parser->token_current->type == TOKEN_DOUBLE || parser->token_current->type == TOKEN_NIL) {
            param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, parser->token_current->type);
            entry->params[0]->type = parser->token_current->type;
            // print_token(parser->token_current);
        } else {
            return WRONG_NUM_TYPE_ERROR;
        }
        parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_COMMA) { // after term has to be comma
            parser_getNewToken(parser);
            continue;
        }
        if (parser->token_current->type != TOKEN_R_BRACKET) {
            return SYNTAX_ERROR;
        }
        gen_write_arg(entry->params[param_idx]);
    }
    return SUCCESS;
}

Error parser_rule_callFunc(Parser_t *parser){
    PRINT_RULE(callFunc);
    //[callFunction] → [functId] ([parameters])
    symtable_entry_t* entry;
    if (table_search_global(parser->symtable, parser->token_current->value.str, &entry) == false) {
        return UNDEFINED_FUNCTION_ERROR;
    }
    char *func_name = parser->token_current->value.str;
    int param_idx = 0;

    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);

    if (!(strcmp(func_name, "write"))) {
        Error err = func_write_call(parser, entry);
        return err;
    }

    // parameters - check both syntax and semantic
    while (entry->params[param_idx] != NULL) {
        int tmp_idx = param_idx + 1;

        bool comma = true;
        if (entry->params[tmp_idx] == NULL) {
            comma = false;
        }

        if (!strcmp(entry->params[param_idx]->name,"_")) { // [expr]
            parser_getNewToken(parser);

            if (parser->token_current->type == TOKEN_IDENTIFIER) {
                symtable_entry_t* paramIdent;

                if (table_search(parser->symtable, parser->token_current->value.str, &paramIdent)) { // find variable in symtable
                    if (paramIdent->type != entry->params[param_idx]->type) { // parameters in func definition and func call must have same type
                        return WRONG_NUM_TYPE_ERROR;
                    }

                    param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, parser->token_current->type);
                } else {
                    return UNDEFINED_VARIABLE_ERROR;
                }
            } else {
                if ((parser->token_current->type == TOKEN_INTEGER && entry->params[param_idx]->type == TOKEN_DT_INT) ||
                        (parser->token_current->type == TOKEN_DOUBLE && entry->params[param_idx]->type == TOKEN_DT_DOUBLE) ||
                        (parser->token_current->type == TOKEN_STRING && entry->params[param_idx]->type == TOKEN_DT_STRING)) {
                    param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, parser->token_current->type);
                } else {
                    return WRONG_NUM_TYPE_ERROR;
                }
            }

            if (comma) { // func has more params 
                parser_getNewToken(parser);
                if (parser->token_current->type != TOKEN_COMMA) {
                    return SYNTAX_ERROR;
                }
            }
        } else { // [name] : [expr]
            parser_getNewToken(parser);
            if (parser->token_current->type == TOKEN_IDENTIFIER && !strcmp(parser->token_current->value.str, entry->params[param_idx]->name)) {

                parser_getNewToken(parser);
                if (parser->token_current->type != TOKEN_COLON) {
                    return SYNTAX_ERROR;
                }

                parser_getNewToken(parser);
                if (parser->token_current->type == TOKEN_IDENTIFIER) {
                    symtable_entry_t* paramIdent;

                    if (table_search_global(parser->symtable, parser->token_current->value.str, &paramIdent)) { // find variable in symtable
                        if (paramIdent->type != entry->params[param_idx]->type) { // parameters in func definition and func call must have same type
                            return WRONG_NUM_TYPE_ERROR;
                        }
                    param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, parser->token_current->type);
                    } else {
                        return UNDEFINED_VARIABLE_ERROR;
                    }
                } else {
                    if ((parser->token_current->type == TOKEN_INTEGER && entry->params[param_idx]->type == TOKEN_DT_INT) ||
                            (parser->token_current->type == TOKEN_DOUBLE && entry->params[param_idx]->type == TOKEN_DT_DOUBLE) ||
                            (parser->token_current->type == TOKEN_STRING && entry->params[param_idx]->type == TOKEN_DT_STRING)) {
                        param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, parser->token_current->type);
                    } else {
                        return WRONG_NUM_TYPE_ERROR;
                    }
                }

                if (comma) { // func has more params 
                    parser_getNewToken(parser);
                    if (parser->token_current->type != TOKEN_COMMA) {
                        return SYNTAX_ERROR;
                    }
                }
            } else {
                return SYNTAX_ERROR;
            }
        }

        param_idx++;
    }

    parser_getNewToken(parser);
    if (parser->token_current->type == TOKEN_R_BRACKET) {
        table_add_scope(parser->symtable);
        add_params_to_scope(parser->symtable,entry);
        gen_func_call(func_name,entry);
        return SUCCESS;
    }

    return SYNTAX_ERROR;
}

Error parser_rule_paramsDef(Parser_t *parser){
    PRINT_RULE(paramsDef);
    //[parameters] →
    //  | ( [name] [id] : [type]  [parameters_seq]*
    //  | ( )
    if (parser_rule_name(parser) == SUCCESS){
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, type);

        GET_NEXT_AND_CALL_RULE(parser, paramsDefSeq);
        goto success;
    }
    else if (parser->token_current->type == TOKEN_R_BRACKET){
        goto success;
    }
    success:
        return SUCCESS;
}

Error parser_rule_paramsDefSeq(Parser_t* parser){
    PRINT_RULE(paramsDefSeq);
    // [parameters_seq] →
    //    | , [name] [id] : [type] [parameters_seq]
    //    | )
    if (parser->token_current->type == TOKEN_COMMA){
        GET_NEXT_AND_CALL_RULE(parser, name);
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CALL_RULE(parser, paramsDefSeq);
        return SUCCESS;
    }
    else if (parser->token_current->type == TOKEN_R_BRACKET){
        return SUCCESS;
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_name(Parser_t *parser){
    PRINT_RULE(name)
    if (parser->token_current->type == TOKEN_IDENTIFIER){
        return SUCCESS;
    }
    else if (parser->token_current->type == TOKEN_UNDERSCORE){
        return SUCCESS;
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_type(Parser_t *parser){
    PRINT_RULE(datatype)
    switch (parser->token_current->type){
        case TOKEN_DT_DOUBLE:
            return SUCCESS;
        case TOKEN_DT_DOUBLE_NIL:
            return SUCCESS;
        case TOKEN_DT_INT:
            return SUCCESS;
        case TOKEN_DT_INT_NIL:
            return SUCCESS;
        case TOKEN_DT_STRING:
            return SUCCESS;
        case TOKEN_DT_STRING_NIL:
            return SUCCESS;
        default:
            return SYNTAX_ERROR;
    }
}

bool expr_var_match(TokenType exprRet, TokenType entry_type) {
    if (exprRet == TOKEN_DT_INT && (entry_type == TOKEN_DT_INT || entry_type == TOKEN_DT_INT_NIL)) {
        return true;
    } else if (exprRet == TOKEN_DT_DOUBLE && (entry_type == TOKEN_DT_DOUBLE || entry_type == TOKEN_DT_DOUBLE_NIL)) {
        return true;
    } else if (exprRet == TOKEN_DT_STRING && (entry_type == TOKEN_DT_STRING || entry_type == TOKEN_DT_STRING_NIL)) {
        return true;
    } else if (exprRet == TOKEN_NIL && (entry_type == TOKEN_DT_INT_NIL || entry_type == TOKEN_DT_DOUBLE_NIL || entry_type == TOKEN_DT_STRING_NIL)) {
        return true;
    }
    return false;
}

Error parser_rule_expr(Parser_t *parser){
    PRINT_RULE(expr);
    //  [expr] →
    //    | ( [expr] )
    //    | [expr] + [expr]
    //    | [expr] * [expr]
    //    | [expr] == [expr]
    //    | [expr] > [expr]
    //    | [expr] !! [expr]
    //    | [expr] ?? [expr]
    //    | [id]
    //    | [literal]
    TokenT *next = NULL;
    TokenType exprRet;
    Error err = bu_read(&next, parser->stack, parser->symtable, &exprRet, parser->if_while); // TODO not this stack

    if (parser->if_while) {
        parser->if_while = false;
    }
    symtable_entry_t* entry;
    if (err == SUCCESS) {
        if (parser->return_in_func) {
            table_search_global(parser->symtable, parser->current_function, &entry);
            if (exprRet != entry->return_type) {
                return WRONG_NUM_TYPE_ERROR;
            }
            parser->return_in_func = false;
            return err;
        } 
        if (parser->find_id_type) { //stmt_assign -> = <expr>  
            table_search(parser->symtable, parser->current_id, &entry);
            if (exprRet == TOKEN_ZERO || exprRet == TOKEN_NIL) { // cant be NIL
                return UNSPECIFIED_TYPE_ERROR;
            } else {
                entry->type = exprRet;
            }
            entry->defined = true;
            parser->find_id_type = false;
        } else if (parser->assign) { // [id] = [expr]
            if (table_search(parser->symtable, parser->current_id, &entry)) {
                if (entry->constant && entry->modified) {
                    return ANOTHER_SEMANTIC_ERROR;
                } else if (entry->constant && !entry->modified) {
                    entry->modified = true;
                    printf("%d\n",entry->modified);
                }
                if (!expr_var_match(exprRet, entry->type)) {
                    return TYPE_COMPATIBILITY_ERROR;
                } 
                parser->assign = false;
            } else {
                return UNDEFINED_VARIABLE_ERROR;
            }
            
        } else { // stmt_assign -> <type> = <expr>  
            table_search(parser->symtable, parser->current_id, &entry);
            if (!expr_var_match(exprRet, entry->type)) {
                return TYPE_COMPATIBILITY_ERROR;
            } 
            if (entry->constant) {
                entry->modified = true;
            }
            entry->defined = true;
        }
        Stack_Push(parser->stack, parser->token_current);
        parser_stashExtraToken(parser, next);
    }
    return err;
}

Error parser_rule_stmtSeq(Parser_t *parser){
    PRINT_RULE(stmtSeq)
    do {
        if (parser->token_current->type == TOKEN_RC_BRACKET) {
            break;
        }
        RuleErr = parser_rule_stmt(parser);
        RETURN_ERROR;
        parser_getNewToken(parser);
    } while (1);
    return SUCCESS;
}

Error parser_rule_stmtMainSeq(Parser_t *parser){
    gen_cont_label(get_cont_label());
    do {
        parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_EOF) {
            break;
        }
        RuleErr = parser_rule_stmt(parser);
        RETURN_ERROR;
    } while (1);
    gen_prog_end(0); 
    return SUCCESS;
}