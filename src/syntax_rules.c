#include "syntax_rules.h"
#include "parser.h"

rule_ret_t parser_rule_id(Parser_t *parser){
    //id ->id
    CHECK_TOKEN_TYPE(parser, TOKEN_IDENTIFIER);
    return RULE_SUCCESS;
}

rule_ret_t parser_rule_funcID(Parser_t *parser){
    //funcId -> id
    CHECK_TOKEN_TYPE(parser, TOKEN_IDENTIFIER);
    return RULE_SUCCESS;
}

rule_ret_t parser_rule_stmt(Parser_t *parser){
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
        parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_LC_BRACKET){
            return RULE_FAIL;
        }
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_RC_BRACKET){
            return RULE_FAIL;
        }
        GET_NEXT_AND_CALL_RULE(parser, elseF); 
    }
    else if (parser->token_current->type == TOKEN_WHILE){
        GET_NEXT_AND_CALL_RULE(parser, expr);
        if (parser->token_current->type != TOKEN_LC_BRACKET){
            return RULE_FAIL;
        }
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        if (parser->token_current->type != TOKEN_RC_BRACKET){
            return RULE_FAIL;
        }
    }
    //stmt -> <defFunc>
    else if (parser->token_current->type == TOKEN_FUNC){
        GET_NEXT_AND_CALL_RULE(parser, defFunc);
    }
    else{
        return RULE_FAIL;
    }
}

rule_ret_t parser_rule_stmtAssign(Parser_t *parser){
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
        parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_ASSIGN){
            return RULE_FAIL;
        }
        GET_NEXT_AND_CALL_RULE(parser, expr);
    }
    else{
        return RULE_FAIL;
    }
}

rule_ret_t parser_rule_elseF(Parser_t *parser){ //
    //else -> else { <stmtSeq> }
    if (parser->token_current->type == TOKEN_ELSE){
        parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_LC_BRACKET){
            return RULE_FAIL;
        }
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_RC_BRACKET){
            return RULE_FAIL;
        }
    }
    //otherwise empty
    else{
        return RULE_SUCCESS;
    }
}

rule_ret_t parser_rule_defFunc(Parser_t *parser){
    //func [funcId] ([parameters]) [func_ret]
    //TODO: add local symtable
    //TODO: add return value check, in case of return type is not void check if return is present
    if (parser->token_current->type == TOKEN_FUNC){
        GET_NEXT_AND_CALL_RULE(parser, funcId);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, parameters);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, funcRet);
    }
    else{
        return RULE_FAIL;
    }
}

rule_ret_t parser_rule_funcRet(Parser_t *parser){
    //func_ret ->
    //  | : <type> { <stmt_seqRet> } (with return check)
    //  | { <stmt_seqRet> } 
    if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        bool returnCheck = false;
        Parser_getNewToken(parser);
        parser_rule_stmtSeqRet(parser, &returnCheck);
        if (!returnCheck){
            return RULE_FAIL; //return function without return
        }
    }   
    else if (parser->token_current->type == TOKEN_LC_BRACKET){
        Parser_getNewToken(parser);
        bool returnCheck = false;
        parser_rule_stmtSeqRet(parser, &returnCheck);
        //TODO: does swift support return in void functions?
    }
    else{
        return RULE_FAIL;
    }
}

rule_ret_t parser_rule_stmtSeqRet(Parser_t *parser, bool retCheck){
    //statment sequence for functions (with return check)
    while (parser->token_current->type != TOKEN_RC_BRACKET){
        Parser_getNewToken(parser);
        if (parser->token_current->type == TOKEN_RETURN){
            retCheck = true;
        }
        parser_rule_stmt(parser);
    }
}

rule_ret_t parser_rule_params(Parser_t *parser){
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
        return RULE_SUCCESS;
    }
}

rule_ret_t parser_rule_type(Parser_t *parser){
    switch (parser->token_current->type){
        case TOKEN_INTEGER:
            return RULE_SUCCESS;
        case TOKEN_STRING:
            return RULE_SUCCESS;
        case TOKEN_NIL:
            return RULE_SUCCESS;
        case TOKEN_DECIMAL: //TODO: premenovat na double
            return RULE_SUCCESS;
        default:
            return RULE_FAIL;
    }
    //type -> int
}

rule_ret_t parser_rule_stmtSeqFunc(Parser_t *parser){
    bool returnCheck = false;
    while (parser->token_current->type != TOKEN_RC_BRACKET){
        if (parser->token_current->type == TOKEN_RETURN){
            returnCheck = true;
        }
        GET_NEXT_AND_CALL_RULE(parser, stmt);
    }
}

rule_ret_t parser_rule_stmtSeq(Parser_t *parser){
    while (parser->token_current->type != TOKEN_EOF){
        GET_NEXT_AND_CALL_RULE(parser, stmt);
    }
}


rule_ret_t parser_rule_expr(Parser_t * parser);

