/**
 * Project: Compliler IFJ23 implementation 
 * File: main.c
 * 
 * @brief implementation of main function
 * 
 * @author Tomáš Zgút xzgutt00
*/

#include "symtable.h"
#include "parser.h"
#include "scanner.h"
#include "errors.h"
#include "prec_table.h"
#include "bu_analysis.h"
#include "utils.h"
#include "code_gen.h"
#include "syntax_rules.h"
#include "first_traverse.h"

int main() {
    Parser_t *parser = calloc(1,sizeof(Parser_t));
    Error err;
    parser_init(parser);

    err = find_allFuncDef(parser);

    if (err != SUCCESS) {
        parser_dtor(parser);
        return err;
    }


    for (int i = 0; i < parser->stack->topIndex; i++) {
        print_token(parser->stack->array[i]);
    }

    gen_prog();
    err = parser_rule_stmtMainSeq(parser);
    
    if (err != SUCCESS) {
        parser_dtor(parser);
        return err;
    }

    parser_dtor(parser);
    return err;
}