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
        GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
        goto success;
    }
    //stmt -> var <id> <stmt_assign>
    else if (parser->token_current->type == TOKEN_VAR){
        GET_NEXT_AND_CALL_RULE(parser, id);
        table_insert(parser->symtable, parser->token_current->value.str, &(parser->current_entry));
        parser->current_entry->declared = true;
        GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
        goto success;
    }
    //stmt -> if <expr> { <stmt>
    else if (parser->token_current->type == TOKEN_IF){ //add local symbol table
        GET_NEXT_AND_CALL_RULE(parser, expr); //with note that expr will handle the brackets
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        table_add_scope(parser->symtable);
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        GET_NEXT_AND_CALL_RULE(parser, elseF);
        goto success;
    }
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
        parser->current_entry->defined = true;
        parser->current_entry->type = parser->token_current->type;
        goto success;
    }
    //stmt_assign -> : <type> = <expr>
    else if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
        parser->current_entry->type = parser->token_current->type;
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
    //TODO: add parameters to local symtable
    table_add_scope(parser->symtable);
    GET_NEXT_AND_CALL_RULE(parser, funcID);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);
    GET_NEXT_AND_CALL_RULE(parser, params);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
    GET_NEXT_AND_CALL_RULE(parser, funcRet);
    table_remove_scope(parser->symtable);
    return SUCCESS;
    return SYNTAX_ERROR;
}

Error parser_rule_funcRet(Parser_t *parser){
    //func_ret ->
    //  | : <type> { <stmt_seqRet> } Return fucntion: Return check for sematic analysis
    //  | { <stmt_stmtVoidSeqRet> } Void fucntion
    // in both cases the right bracket is checked in the stmtSeqRet
    if (parser->token_current->type == TOKEN_COLON){
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
    GET_NEXT_AND_CALL_RULE(parser, funcID);
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);
    GET_NEXT_AND_CALL_RULE(parser, params);
    return SUCCESS;
}

Error parser_rule_params(Parser_t *parser){
    //[parameters] →
    //  | ( [id] : [type]  [parameters_seq]*
    //  | ( )
    GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
    parser_getNewToken(parser);
    if (parser->token_current->type == TOKEN_IDENTIFIER){
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CALL_RULE(parser, paramsSeq);
        goto success;
    }
    else if (parser->token_current->type == TOKEN_L_BRACKET){
        goto success;
    }
    success:
        return SUCCESS;
}

Error parser_rule_paramsSeq(Parser_t* parser){
    // [parameters_seq] →
    //    | , [id] : [type] [parameters_seq]
    //    | )
    if (parser->token_current->type == TOKEN_COMMA){
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CALL_RULE(parser, paramsSeq);
        return SUCCESS;
    }
    else if (parser->token_current->type == TOKEN_R_BRACKET){
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
        case TOKEN_DOUBLE:
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
    //    | [expr] !
    //    | [expr] ?? [expr]
    //    | [id]
    //    | [literal]
    parser_getNewToken(parser);
    TokenT *next = malloc(sizeof(TokenT));
    next->type = TOKEN_ZERO;
    // TODO vymaz
    symtable_t symTab;
    table_init(&symTab);
    Error err = bu_read(&next, &symTab);
    Stack_Push(parser->stack, parser->token_current);
    parser_stashExtraToken(parser, next);
}

Error parser_rule_stmtSeq(Parser_t *parser){
    table_add_scope(parser->symtable); //add global symtable
    while (parser->token_current->type != TOKEN_EOF){
        parser_getNewToken(parser);
        if (parser_rule_stmt(parser) == SYNTAX_ERROR){
            return SYNTAX_ERROR;
        }
    }
    return SUCCESS;
}