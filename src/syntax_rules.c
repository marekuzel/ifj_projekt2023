#include "syntax_rules.h"
#include "parser.h"

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
        GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
    }
    //stmt -> var <id> <stmt_assign>
    else if (parser->token_current->type == TOKEN_VAR){
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CALL_RULE(parser, stmtAssign);
    }
    //stmt -> if <expr> <stmt>
    else if (parser->token_current->type == TOKEN_IF){ //add local symbol table
        GET_NEXT_AND_CALL_RULE(parser, expr); //with note that expr will handle the brackets
        Parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_LC_BRACKET){
            return SYNTAX_ERROR;
        }
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        Parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_RC_BRACKET){
            return SYNTAX_ERROR;
        }
        GET_NEXT_AND_CALL_RULE(parser, elseF); 
    }
    else if (parser->token_current->type == TOKEN_WHILE){
        GET_NEXT_AND_CALL_RULE(parser, expr);
        if (parser->token_current->type != TOKEN_LC_BRACKET){
            return SYNTAX_ERROR;
        }
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        if (parser->token_current->type != TOKEN_RC_BRACKET){
            return SYNTAX_ERROR;
        }
    }
    //stmt -> <defFunc>
    else if (parser->token_current->type == TOKEN_FUNC){
        GET_NEXT_AND_CALL_RULE(parser, defFunc);
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_stmtAssign(Parser_t *parser){
    //stmt_assign -> = <expr>
    if (parser->token_current->type == TOKEN_ASSIGN){
        GET_NEXT_AND_CALL_RULE(parser, expr);
    }
    //stmt_assign -> : <type>
    else if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
    }
    //stmt_assign -> : <type> = <expr>
    else if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
        Parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_ASSIGN){
            return SYNTAX_ERROR;
        }
        GET_NEXT_AND_CALL_RULE(parser, expr);
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_elseF(Parser_t *parser){ //
    //else -> else { <stmtSeq> }
    if (parser->token_current->type == TOKEN_ELSE){
        Parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_LC_BRACKET){
            return SYNTAX_ERROR;
        }
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        Parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_RC_BRACKET){
            return SYNTAX_ERROR;
        }
    }
    //otherwise empty
    else{
        return SUCCESS;
    }
}

Error parser_rule_defFunc(Parser_t *parser){
    //func [funcId] ([parameters]) [func_ret]
    //TODO: add local symtable
    //TODO: add return value check, in case of return type is not void check if return is present
    if (parser->token_current->type == TOKEN_FUNC){
        GET_NEXT_AND_CALL_RULE(parser, funcID);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, params);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, funcRet);
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_funcRet(Parser_t *parser){
    //func_ret ->
    //  | : <type> { <stmt_seqRet> } Return fucntion: Return check for sematic analysis
    //  | { <stmt_seqRet> } Void fucntion
    // in both cases the right bracket is checked in the stmtSeqRet
    if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, stmtSeqRet);
    }   
    else if (parser->token_current->type == TOKEN_LC_BRACKET){
        Parser_getNewToken(parser);
        parser_rule_stmtSeqRet(parser);
    }
    else{
        return SYNTAX_ERROR;
    }
}

Error parser_rule_stmtSeqRet(Parser_t *parser){
    //statment sequence for functions (with return check)
    while (parser->token_current->type != TOKEN_RC_BRACKET){
        Parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_RETURN){
        }
        else{
            GET_NEXT_AND_CALL_RULE(parser, stmt);
        }
    }
}

Error parser_rule_params(Parser_t *parser){
    //[parameters] →
    //  | [id] : [type] [parameters_seq]*
    //  | // empty
    if (parser->token_current->type == TOKEN_IDENTIFIER){
        GET_NEXT_AND_CALL_RULE(parser, id);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_COLON);
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CALL_RULE(parser, parametersSeq);
    }
    else{
        return SUCCESS;
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
    Parser_getNewToken(parser);
    TokenT *next = malloc(sizeof(TokenT));
    next->type = TOKEN_ZERO;
    return bu_read(&next);
    Stack_Push(parser->stack, parser->token_current);
    parser->token_current = next;
}

Error parser_rule_stmtSeq(Parser_t *parser){
    while (parser->token_current->type != TOKEN_EOF){
        Parser_getNewToken(parser);
        return parser_rule_stmt(parser);
    }
}