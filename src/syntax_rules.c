#include "syntax_rules.h"
#include "parser.h"
/*
*****************************
Implementaion of syntax rules
*****************************
*/

Error parser_rule_id(Parser_t *parser){
    //id ->id
    CHECK_TOKEN_TYPE(parser, TOKEN_IDENTIFIER);
    return SUCCESS;
}

Error parser_rule_funcID(Parser_t *parser){
    //funcId -> id
    CHECK_TOKEN_TYPE(parser, TOKEN_IDENTIFIER);
    return SUCCESS;
}

Error parser_rule_stmt(Parser_t *parser){
    //stmt -> let <id> <stmt_assign>
    if (parser->token_current->type == TOKEN_LET){
        GET_NEXT_AND_CALL_RULE(parser, id);

        table_insert(parser->symtable,parser->token_current->value.str,&(parser->current_entry));
        parser->current_entry->declared = true;
        parser->current_entry->constant = true;

        GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
        goto success;
    }
    //stmt -> var <id> <stmt_assign>
    else if (parser->token_current->type == TOKEN_VAR){
        GET_NEXT_AND_CALL_RULE(parser, id);

        //add variable to symtable and assign that its declared
        table_insert(parser->symtable, parser->token_current->value.str, &(parser->current_entry));
        parser->current_entry->declared = true;
        parser->current_entry->constant = false;

        GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
        goto success;
    }
    //stmt -> 
     // | if <expr> { <stmtSeq> }
    //  | if let [id] [stmtAssign] [stmtSeq] [stmt_else]
    else if (parser->token_current->type == TOKEN_IF){ //add local symbol table
        table_add_scope(parser->symtable);
        parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_LET){
            GET_NEXT_AND_CALL_RULE(parser, id);
            GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_RC_BRACKET);
            GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
            table_remove_scope(parser->symtable);
        }
        else{
            GET_NEXT_AND_CALL_RULE(parser, expr); //with note that expr will handle the brackets
            GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
            GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
            GET_NEXT_AND_CALL_RULE(parser, elseF);
            table_remove_scope(parser->symtable);
        }
        goto success;
    }
    //stmt -> while [expr] { [stmt_seqFunc] }
    else if (parser->token_current->type == TOKEN_WHILE){
        GET_NEXT_AND_CALL_RULE(parser, expr);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);

        table_add_scope(parser->symtable);
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        table_remove_scope(parser->symtable);

        goto success;
    }
    //stmt -> <defFunc>
    else if (parser->token_current->type == TOKEN_FUNC){
        GET_NEXT_AND_CALL_RULE(parser, defFunc);
        goto success;
    }
    //stmt -> [id] = [expr]
    else if (parser->token_current->type == TOKEN_IDENTIFIER){
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_ASSIGN);
        GET_NEXT_AND_CALL_RULE(parser, expr);
        goto success;
    }
    else{
        return SYNTAX_ERROR;
    }
    success:
        return SUCCESS;
}

Error parser_rule_stmtAssign(Parser_t *parser){
    //stmt_assign -> = <expr>
    if (parser->token_current->type == TOKEN_ASSIGN){
        GET_NEXT_AND_CALL_RULE(parser, expr);
        parser->current_entry->defined = true;
        goto success;
    }
    //stmt_assign -> : <type>
    else if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
        parser->current_entry->type = parser->token_current->type;
        goto success;
    }
    //stmt_assign -> : <type> = <expr>
    else if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
        parser->current_entry->type = parser->token_current->type;
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_ASSIGN);
        GET_NEXT_AND_CALL_RULE(parser, expr);
        parser->current_entry->defined = true;
        goto success;
    }
    else{
        return SYNTAX_ERROR;
    }
    success:
        return SUCCESS;
}

Error parser_rule_paramsCall (Parser_t *parser){
    //     [parametersCall] →
    //    | [name] : [expr] [parameters_seqCall]
    //    | empty
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
    parser_getNewToken(parser);
    if (parser->token_current->type == TOKEN_IDENTIFIER){
        GET_NEXT_AND_CALL_RULE(parser, id); //name is id from syntax pow

        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, expr);
        
        GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
        goto success;
    }
    else if (parser->token_current->type == TOKEN_L_BRACKET){
        goto success;
    }
    success:
        return SUCCESS;
}

Error parser_rule_paramsCallSeq (Parser_t * parser){
    // [parameters_seqCall] →
    //    | , [name] : [expr] [parameters_seq]
    //    | // empty
    if (parser->token_current->type == TOKEN_COMMA){
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, expr);
        GET_NEXT_AND_CALL_RULE(parser, paramsCallSeq);
        return SUCCESS;
    }
    else if (parser->token_current->type == TOKEN_R_BRACKET){
        table_remove_scope(parser->symtable);
        return SUCCESS;
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_elseF(Parser_t *parser){
    //else -> else { <stmtSeq>
    if (parser->token_current->type == TOKEN_ELSE){
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        table_add_scope(parser->symtable);
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        table_remove_scope(parser->symtable);
    }
    //otherwise empty
    return SUCCESS;
}

Error parser_rule_defFunc(Parser_t *parser){
    //func [funcId] ([parameters]) [func_ret]
    GET_NEXT_AND_CALL_RULE(parser, funcID);
    table_insert_global(parser->symtable, parser->token_current->value.str, &(parser->current_entry));
    if (param_buffer_init((parser)->buffer) == BUFF_INIT_FAIL)return SYNTAX_ERROR;

    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);
    GET_NEXT_AND_CALL_RULE(parser, paramsDef);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
    GET_NEXT_AND_CALL_RULE(parser, funcRet);

    return SUCCESS;
    return SYNTAX_ERROR;
}

Error parser_rule_funcRet(Parser_t *parser){
    if (parser->token_current->type == TOKEN_ARROW){
        GET_NEXT_AND_CALL_RULE(parser, type);
        parser->current_entry->return_type = parser->token_current->type;
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
    // [stmt_seqFunc] → 
    //    | [stmt]
    //    | return [expr]
    while (parser->token_current->type != TOKEN_RC_BRACKET){
        parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_RETURN){
            GET_NEXT_AND_CALL_RULE(parser, expr);
        }
        else{
            if (parser_rule_stmt(parser) == SYNTAX_ERROR){ 
                return SYNTAX_ERROR;
            }
        }
    }
    return SUCCESS;
}

Error parser_rule_stmtVoidSeqRet(Parser_t *parser){
    // [stmt_seqVoidSeqRet] → 
    //    | [stmt]
    //    | return
    while (parser->token_current->type != TOKEN_RC_BRACKET){
        parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_RETURN){
            continue;
        }
        else{
            if (parser_rule_stmt(parser) == SYNTAX_ERROR){ 
                return SYNTAX_ERROR;
            }
        }
    }
    return SUCCESS;
}

Error parser_rule_callFunc(Parser_t *parser){
    //[callFunction] → [functId] ([parameters])
    table_add_scope(parser->symtable);
    GET_NEXT_AND_CALL_RULE(parser, funcID);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);

    GET_NEXT_AND_CALL_RULE(parser, paramsCall);
    return SUCCESS;
}

Error parser_rule_paramsDef(Parser_t *parser){
    //[parameters] →
    //  | ( [name] [id] : [type]  [parameters_seq]*
    //  | ( )
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
    parser_getNewToken(parser);
    if (parser->token_current->type == TOKEN_IDENTIFIER){
        GET_NEXT_AND_CALL_RULE(parser, id); //name is id from syntax pow
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, type);

        parser_createParam(parser);

        GET_NEXT_AND_CALL_RULE(parser, paramsDefSeq);
        goto success;
    }
    else if (parser->token_current->type == TOKEN_L_BRACKET){
        goto success;
    }
    success:
        return SUCCESS;
}

Error parser_rule_paramsDefSeq(Parser_t* parser){
    // [parameters_seq] →
    //    | , [name] [id] : [type] [parameters_seq]
    //    | )
    if (parser->token_current->type == TOKEN_COMMA){
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, type);

        parser_createParam(parser);

        GET_NEXT_AND_CALL_RULE(parser, paramsDefSeq);
        return SUCCESS;
    }
    else if (parser->token_current->type == TOKEN_R_BRACKET){
        parser->current_entry->params = param_buffer_export(parser->buffer);
        if (parser->current_entry->params == NULL) return INTERNAL_COMPILER_ERROR;
        parser->current_entry->type = TOKEN_FUNC;
        parser->current_entry->defined = true;
        parser->current_entry->declared = true;
        return SUCCESS;
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_type(Parser_t *parser){
    switch (parser->token_current->type){
        case TOKEN_INTEGER:
            return SUCCESS;
        case TOKEN_STRING:
            return SUCCESS;
        case TOKEN_NIL:
            return SUCCESS;
        default:
            return SYNTAX_ERROR;
    }
}

Error parser_rule_expr(Parser_t *parser){
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
    parser_getNewToken(parser);
    TokenT *next = malloc(sizeof(TokenT));
    next->type = TOKEN_ZERO;
    return bu_read(&next);
    Stack_Push(parser->stack, parser->token_current);
    parser_stashExtraToken(parser, next);
    return err;
}

Error parser_rule_stmtSeq(Parser_t *parser){
    while (parser->token_current->type != TOKEN_RC_BRACKET){
        parser_getNewToken(parser);
        if (parser_rule_stmt(parser) == SYNTAX_ERROR){
            return SYNTAX_ERROR;
        }
    }
    return SUCCESS;
}

Error parser_rule_stmtMainSeq(Parser_t *parser){
    while (parser->token_current->type != TOKEN_EOF){
        parser_getNewToken(parser);
        if (parser_rule_stmt(parser) == SYNTAX_ERROR){
            return SYNTAX_ERROR;
        }
    }
    return SUCCESS;
}