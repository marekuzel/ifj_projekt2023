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

//[id] → *id*
Error parser_rule_id(Parser_t *parser){
    PRINT_RULE(id);
    CHECK_TOKEN_TYPE(parser, TOKEN_IDENTIFIER, SYNTAX_ERROR);
    parser->current_id = parser->token_current->value.str;
    return SUCCESS;
}

//[funcId] → *funcId*
Error parser_rule_funcID(Parser_t *parser){
    PRINT_RULE(funcID);
    CHECK_TOKEN_TYPE(parser, TOKEN_IDENTIFIER, SYNTAX_ERROR);
    parser->current_function = parser->token_current->value.str;
    return SUCCESS;
}

//let [id] [stmt_assign]
Error parser_rule_let(Parser_t *parser){
    GET_NEXT_AND_CALL_RULE(parser, id);
    if (table_search_local(parser->symtable,parser->current_id)) {
        return UNDEFINED_FUNCTION_ERROR;
    }
    table_insert(parser->symtable,parser->current_id,&(parser->current_entry));
    parser->current_entry->declared = true;
    parser->current_entry->constant = true;
    gen_def_var(parser->current_id,is_global(parser->symtable,parser->current_id),parser->current_entry->type);
    GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
    return SUCCESS;
}

//var [id] [stmt_assign]
Error parser_rule_var(Parser_t *parser){
    GET_NEXT_AND_CALL_RULE(parser, id);
    if (table_search_local(parser->symtable,parser->current_id)) {
            return UNDEFINED_FUNCTION_ERROR;
    }
    table_insert(parser->symtable, parser->current_id, &(parser->current_entry));
    parser->current_entry->declared = true;
    parser->current_entry->constant = false;
    gen_def_var(parser->current_id,is_global(parser->symtable,parser->current_id),parser->current_entry->type);
    GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
    return SUCCESS;
    }

//if let [id] = [expr] [stmt_else]
Error parser_rule_ifr(Parser_t *parser){
    parser->if_while = true;
    int cnd_label = get_cond_label();
    CALL_RULE(parser, expr);
    
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
    table_remove_scope(parser->symtable);
    gen_end_label(IF_L,cnd_label);
    return SUCCESS;
}

//if [expr] { [stmt_seqFunc] } [stmt_else]
Error parser_rule_ifLet(Parser_t *parser){
    GET_NEXT_AND_CALL_RULE(parser, id);
    symtable_entry_t* entry;
    if (table_search(parser->symtable, parser->current_id, &entry)) {
        switch (entry->type) {
            case TOKEN_DT_DOUBLE_NIL: case TOKEN_DT_INT_NIL: case TOKEN_DT_STRING_NIL:
                break;
            default: 
                return ANOTHER_SEMANTIC_ERROR;
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
    entry->constant = true;
    gen_local_scope(parser->symtable);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
    GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
    gen_jmp(IF_END_L,cond_label);
    gen_cond_else_label(cond_label);
    entry->type++; //changing datatype back to with ?
    entry->constant = false;
    parser_getNewToken(parser);
    if (currTokenType == TOKEN_ELSE){
        CALL_RULE(parser, elseF);
    }
    else {
        parser_stashExtraToken(parser, parser->token_current);
    }
    gen_end_label(IF_L,cond_label);
    gen_drop_local_scope(parser->symtable);
    table_remove_scope(parser->symtable);
    return SUCCESS;
}

//stmt -> while [expr] { [stmt_seqFunc] }
Error parser_rule_whileS(Parser_t *parser){
    parser->if_while = true;
    int loop_label = get_loop_label();
    gen_loop_label(loop_label);
    CALL_RULE(parser, expr);
    gen_cnd_jump(LOOP_END_L,loop_label);
    table_add_scope(parser->symtable);
    gen_local_scope(parser->symtable);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        
    GET_NEXT_AND_CALL_RULE(parser, stmtSeq);

    gen_drop_local_scope(parser->symtable);
    table_remove_scope(parser->symtable);
    gen_jmp(LOOP_L,loop_label);
    gen_end_label(LOOP_L,loop_label);
    return SUCCESS;
}

Error parser_rule_elseF(Parser_t *parser){
    PRINT_RULE(elsef);
    if (currTokenType == TOKEN_ELSE){
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);

        table_add_scope(parser->symtable);
        gen_local_scope(parser->symtable);

        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
    }
    //otherwise empty
    return SUCCESS;
}

Error parser_rule_type(Parser_t *parser){
    PRINT_RULE(datatype)
    switch (currTokenType){
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
    symtable_entry_t* entry;

    if (parser->assign) { // [id] = [expr]
        if (table_search(parser->symtable, parser->current_id, &entry)) {
            if (entry->type == TOKEN_DT_DOUBLE || entry->type == TOKEN_DT_DOUBLE_NIL) {
                parser->assign_to_double = true;
            }
        } else {
            return UNDEFINED_VARIABLE_ERROR;
        }
    }

    Error err = bu_read(&next, parser->stack, parser->symtable, &exprRet, parser->if_while, parser->assign_to_double); 

    parser->assign_to_double = false;

    if (parser->if_while) {
        parser->if_while = false;
        // return err;
    }
    
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
            if (exprRet == TOKEN_NIL) { // cant be NIL
                return UNSPECIFIED_TYPE_ERROR;
            } else if (exprRet == TOKEN_ZERO) {
                return TYPE_COMPATIBILITY_ERROR;
            } else {
                entry->type = exprRet;
            }
            entry->defined = true;
            parser->find_id_type = false;
        } else if (parser->assign) { // [id] = [expr]
            table_search(parser->symtable, parser->current_id, &entry);
            if (entry->constant && entry->defined) {
                return ANOTHER_SEMANTIC_ERROR;
            } else if (entry->constant && !entry->defined) {
                entry->defined = true;
            }
            if (!_expr_var_match(exprRet, entry->type)) {
                return TYPE_COMPATIBILITY_ERROR;
            } 
            parser->assign = false;
            
        } else if (parser->assign_type_expr){ // stmt_assign -> <type> = <expr>  
            table_search(parser->symtable, parser->current_id, &entry);
            if (!_expr_var_match(exprRet, entry->type)) {
                return TYPE_COMPATIBILITY_ERROR;
            } 
            if (entry->constant) {
                entry->modified = true;
            }
            entry->defined = true;
            parser->assign_type_expr = false;
        }
        Stack_Push(parser->stack, parser->token_current);
        parser_stashExtraToken(parser, next);
    }
    return err;
}

/*
********************************
Definition of the funciton rules
********************************
*/

Error parser_rule_defFunc(Parser_t *parser){
    int cont_label = get_cont_label();
    jump_cont_label(cont_label);
    //func [funcId] ([parameters]) [func_ret]
    // GET_NEXT_AND_CALL_RULE(parser, funcID);
    CALL_RULE(parser, funcID);
    symtable_entry_t *tmp_entry;
    table_search_global(parser->symtable,parser->token_current->value.str,&tmp_entry);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);
    GET_NEXT_AND_CALL_RULE(parser, paramsDef);
    table_add_scope(parser->symtable);
    add_params_to_scope(parser->symtable,tmp_entry);
    gen_func_def(parser->current_function);
    GET_NEXT_AND_CALL_RULE(parser, funcRet);
    table_remove_scope(parser->symtable);
    gen_func_return();
    gen_cont_label(cont_label);
    return SUCCESS;
}

Error parser_rule_funcRet(Parser_t *parser){
    PRINT_RULE(funcRet);
    if (currTokenType == TOKEN_ARROW){
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, stmtSeqRet);
        return SUCCESS;
    }   
    else if (currTokenType == TOKEN_LC_BRACKET ){
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
        if (currTokenType == TOKEN_RETURN){
            parser->return_in_func = true;
            gen_func_return();
            CALL_RULE(parser, expr);
            parser->stack->bottomIndex--;
            parser_getNewToken(parser);
            continue;
        }
        else if (currTokenType == TOKEN_RC_BRACKET){
            return SUCCESS;
        }
        else{
            CALL_RULE(parser, stmt);
        }
    return SUCCESS;
    }
}

Error parser_rule_stmtVoidSeqRet(Parser_t *parser){
    PRINT_RULE(parser_rule_stmtVoidSeqRet);
    // [stmt_seqVoidSeqRet] → 
    //    | [stmt]
    //    | return
    while (currTokenType != TOKEN_RC_BRACKET){
        if (currTokenType == TOKEN_RETURN){
            gen_func_return();
            parser_getNewToken(parser);
            continue;
        }
        else{
            CALL_RULE(parser, stmt);
        }
        parser_getNewToken(parser);
    }
    return SUCCESS;
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
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_R_BRACKET){
        return SUCCESS;
    }
    return SYNTAX_ERROR;
}

Error parser_rule_paramsDefSeq(Parser_t* parser){
    PRINT_RULE(paramsDefSeq);
    // [parameters_seq] →
    //    | , [name] [id] : [type] [parameters_seq]
    //    | )
    if (currTokenType == TOKEN_COMMA){
        GET_NEXT_AND_CALL_RULE(parser, name);
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CALL_RULE(parser, paramsDefSeq);
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_R_BRACKET){
        return SUCCESS;
    }
    else{
        return SYNTAX_ERROR;
    }
}

/**
*******************
Call function rules
*******************
 */

Error _id_symtable(Parser_t *parser, param_t* param) {
    symtable_entry_t* paramIdent;
    if (table_search(parser->symtable, parser->token_current->value.str, &paramIdent)) { // find variable in symtable
        if (paramIdent->defined == false){
            return UNDEFINED_VARIABLE_ERROR;
        }
        else if (paramIdent->type != TOKEN_FUNC) {
            param_value_init(parser->symtable, param, parser->token_current->value, currTokenType);
            param->type = TOKEN_VAR;
            return SUCCESS;
        }
        else if (paramIdent->declared == false) {
            return UNSPECIFIED_TYPE_ERROR;
        }
        else {
            return WRONG_NUM_TYPE_ERROR;
        } 
    }
    else {
        return UNDEFINED_VARIABLE_ERROR;
    }
}

Error func_write_call(Parser_t *parser, symtable_entry_t* entry) { // write(term1, term3, .. , termn)
    PRINT_RULE(func_write_call);
    parser_getNewToken(parser);

    param_t* param = entry->params[0];
    while (currTokenType != TOKEN_R_BRACKET) {
        if (currTokenType == TOKEN_IDENTIFIER) {
            RuleErr =_id_symtable(parser, param);
            if (RuleErr != SUCCESS) return RuleErr;
        } else if (currTokenType == TOKEN_INTEGER || currTokenType == TOKEN_STRING || 
                    currTokenType == TOKEN_DOUBLE || currTokenType == TOKEN_NIL) {
            param_value_init(parser->symtable, param, parser->token_current->value, currTokenType);
            param->type = currTokenType;
        } else {
            return WRONG_NUM_TYPE_ERROR;
        }
        gen_write_arg(param);
        parser_getNewToken(parser);
        if (currTokenType == TOKEN_COMMA) { // after term has to be comma
            parser_getNewToken(parser);
            continue;
        }
        CHECK_TOKEN_TYPE(parser, TOKEN_R_BRACKET, SYNTAX_ERROR);
    }
    return SUCCESS;
}

Error parser_rule_paramsCallSeq (Parser_t * parser){
    if (currTokenType == TOKEN_COMMA){
        parser_getNewToken(parser);
        if (currTokenType == TOKEN_IDENTIFIER){
            GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
            CALL_RULE(parser, expr);
            GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
            return SUCCESS;
        }
        else{
            parser->stack->bottomIndex--;
            CALL_RULE(parser, expr);
            GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
            return SUCCESS;
        }
    }
    else if (currTokenType == TOKEN_R_BRACKET){
        return SUCCESS;
    }
    return SYNTAX_ERROR;
}

Error parser_rule_paramsCall (Parser_t *parser){
    PRINT_RULE(paramsCall);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
    parser_getNewToken(parser);
    if (currTokenType == TOKEN_IDENTIFIER){
        parser->current_function = parser->token_current->value.str;
        GET_NEXT_AND_CALL_RULE(parser, id); //name is id from syntax pow

        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        CALL_RULE(parser, expr);
        
        GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_L_BRACKET){
        return SUCCESS;
    }
    else {
        parser->stack->bottomIndex--;
        CALL_RULE(parser, expr);
        GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
    }
    return SYNTAX_ERROR;
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
    table_add_scope(parser->symtable);
    // parameters - check both syntax and semantic
    while (entry->params[param_idx] != NULL) {
        int tmp_idx = param_idx + 1;

        bool comma = true;
        if (entry->params[tmp_idx] == NULL) {
            comma = false;
        }
        
        if (!strcmp(entry->params[param_idx]->name,"_")) { // [expr]
            parser_getNewToken(parser);
            if (currTokenType == TOKEN_IDENTIFIER) {
                symtable_entry_t* paramIdent;

                if (table_search(parser->symtable, parser->token_current->value.str, &paramIdent)) { // find variable in symtable
                    if (paramIdent->type != entry->params[param_idx]->type) { // parameters in func definition and func call must have same type
                        return WRONG_NUM_TYPE_ERROR;
                    }

                    param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, currTokenType);
                } else {
                    return UNDEFINED_VARIABLE_ERROR;
                }
            } else {
                if ((currTokenType == TOKEN_INTEGER && entry->params[param_idx]->type == TOKEN_DT_INT) ||
                        (currTokenType == TOKEN_DOUBLE && entry->params[param_idx]->type == TOKEN_DT_DOUBLE) ||
                        (currTokenType == TOKEN_STRING && entry->params[param_idx]->type == TOKEN_DT_STRING)) {
                    param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, currTokenType);
                } else {
                    return WRONG_NUM_TYPE_ERROR;
                }
            }

            if (comma) { // func has more params 
                parser_getNewToken(parser);
                if (currTokenType != TOKEN_COMMA) {
                    return SYNTAX_ERROR;
                }
            }
        } else { // [name] : [expr]
            parser_getNewToken(parser);
            if (currTokenType == TOKEN_IDENTIFIER && !strcmp(parser->token_current->value.str, entry->params[param_idx]->name)) {

                parser_getNewToken(parser);
                if (currTokenType != TOKEN_COLON) {
                    return SYNTAX_ERROR;
                }

                parser_getNewToken(parser);
                if (currTokenType == TOKEN_IDENTIFIER) {
                    symtable_entry_t* paramIdent;

                    if (table_search_global(parser->symtable, parser->token_current->value.str, &paramIdent)) { // find variable in symtable
                        if (paramIdent->type != entry->params[param_idx]->type) { // parameters in func definition and func call must have same type
                            return WRONG_NUM_TYPE_ERROR;
                        }
                        param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, currTokenType);
                    } else {
                        return UNDEFINED_VARIABLE_ERROR;
                    }
                } else {
                    if ((currTokenType == TOKEN_INTEGER && entry->params[param_idx]->type == TOKEN_DT_INT) ||
                            (currTokenType == TOKEN_DOUBLE && entry->params[param_idx]->type == TOKEN_DT_DOUBLE) ||
                            (currTokenType == TOKEN_STRING && entry->params[param_idx]->type == TOKEN_DT_STRING)) {
                        param_value_init(parser->symtable, entry->params[param_idx], parser->token_current->value, currTokenType);
                    } else {
                        return WRONG_NUM_TYPE_ERROR;
                    }
                }

                if (comma) { // func has more params 
                    parser_getNewToken(parser);
                    if (currTokenType != TOKEN_COMMA) {
                        return SYNTAX_ERROR;
                    }
                }
            } else {
                return WRONG_NUM_TYPE_ERROR;
            }
        }

        param_idx++;
    }

    parser_getNewToken(parser);
    if (currTokenType == TOKEN_R_BRACKET) {
        table_remove_scope(parser->symtable);
        gen_func_call(func_name,entry);
        return SUCCESS;
    }

    return WRONG_NUM_TYPE_ERROR;
}

Error parser_rule_name(Parser_t *parser){
    PRINT_RULE(name)
    if (currTokenType == TOKEN_IDENTIFIER){
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_UNDERSCORE){
        return SUCCESS;
    }
    else{
        return SYNTAX_ERROR;
    }
}

/**
*******************
Statment rules
*******************
 */

//[stmt] →
Error parser_rule_stmt(Parser_t *parser){
    PRINT_RULE(stmt);
    if (currTokenType == TOKEN_LET){
        PRINT_RULE(Let);
        CALL_RULE(parser, let);
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_VAR){
        PRINT_RULE(var);
        CALL_RULE(parser, var);
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_IF){
        parser_getNewToken(parser);
        if (currTokenType == TOKEN_LET){
            PRINT_RULE(ifLet);
            CALL_RULE(parser, ifLet);
        }
        else{
            parser->stack->bottomIndex--; //return token back to stack
            CALL_RULE(parser, ifr);
        }
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_WHILE){
        PRINT_RULE (whileS);
        CALL_RULE(parser, whileS);
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_FUNC){
        PRINT_RULE(defFunc);
        GET_NEXT_AND_CALL_RULE(parser, defFunc);
        return SUCCESS;
    }
    else if (currTokenType == TOKEN_IDENTIFIER){
        symtable_entry_t *tmp;
        if (table_search_global(parser->symtable,parser->token_current->value.str,&tmp) && tmp->type == TOKEN_FUNC) {
            parser->current_function = parser->token_current->value.str;
            CALL_RULE(parser, callFunc);
            return SUCCESS;
        } 
        else {
            PRINT_RULE(stmtID or uninitialized func);
            parser->current_id = parser->token_current->value.str;
            parser_getNewToken(parser);
            if (currTokenType == TOKEN_L_BRACKET){
                return UNDEFINED_FUNCTION_ERROR;
            }
            else if (currTokenType == TOKEN_ASSIGN){
                parser->assign = true;
                CALL_RULE(parser, expr);
                gen_assignment(parser->current_id,is_global(parser->symtable,parser->current_id));
                return SUCCESS;
            }
            else{
                return SYNTAX_ERROR;
            }
        }
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_stmtAssign(Parser_t *parser){
    PRINT_RULE(stmtAssign);
    //stmt_assign -> = <expr>
    if (currTokenType == TOKEN_ASSIGN){
        parser->find_id_type = true;
        CALL_RULE(parser, expr);
        gen_assignment(parser->current_id,is_global(parser->symtable,parser->current_id));
        if (parser->current_entry->defined == false) {
            parser->current_entry->defined = true;
        } else {
            parser->current_entry->modified = true;
        }
        return SUCCESS;
    }
    //stmt_assign -> : <type>
    //stmt_assign -> : <type> = [expr]
    else if (currTokenType == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
        parser->current_entry->type = currTokenType;
        if (currTokenType == TOKEN_DT_DOUBLE || currTokenType == TOKEN_DT_DOUBLE) {
            parser->assign_to_double = true;
        }
        parser_getNewToken(parser);
        if (currTokenType == TOKEN_ASSIGN){
            parser->current_entry->defined = true;
            parser->assign_type_expr = true;
            CALL_RULE(parser, expr);
            gen_assignment(parser->current_id,is_global(parser->symtable,parser->current_id));
            return SUCCESS;
        }
        else{
            parser_stashExtraToken(parser, parser->token_current);
            return SUCCESS;
        }
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_stmtSeq(Parser_t *parser){
    PRINT_RULE(stmtSeq)
    do {
        if (currTokenType == TOKEN_RC_BRACKET) {
            break;
        }
        CALL_RULE(parser, stmt);
        parser_getNewToken(parser);
    } while (1);
    return SUCCESS;
}

Error parser_rule_stmtMainSeq(Parser_t *parser){
    gen_cont_label(get_cont_label());
    do {
        parser_getNewToken(parser);
        if (currTokenType == TOKEN_EOF) {
            break;
        }
        CALL_RULE(parser, stmt);
    } while (1);
    gen_prog_end(0); 
    return SUCCESS;
}