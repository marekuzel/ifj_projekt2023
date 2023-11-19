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
    //else -> else <stmt>
    if (parser->token_current->type == TOKEN_ELSE){
        parser_getNewToken(parser);
        if (parser->token_current->type != TOKEN_LC_BRACKET){
            return RULE_FAIL;
        }
        GET_NEXT_AND_CALL_RULE(parser, stmt_seq);
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
    //func [funcId] ([parameters]) [func_ret] { [stmt_seq] }
    //TODO: add local symtable
    //TODO: add return value check, in case of return type is not void check if return is present
    if (parser->token_current->type == TOKEN_FUNC){
        GET_NEXT_AND_CALL_RULE(parser, funcId);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_L_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, parameters);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_R_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, funcRet);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_LC_BRACKET);
        GET_NEXT_AND_CALL_RULE(parser, stmtSeq);
        GET_NEXT_AND_CHECK_TYPE(parser, TOKEN_RC_BRACKET);
    }
    else{
        return RULE_FAIL;
    }
}

rule_ret_t parser_rule_funcRet(Parser_t *parser){
    //func_ret -> : <type>
    if (parser->token_current->type == TOKEN_COLON){
        GET_NEXT_AND_CALL_RULE(parser, type);
    }
    //otherwise empty
    else{
        return RULE_SUCCESS;
    }
}

rule_ret_t parser_rule_expr(Parser_t * parser);

