#include "symtable.h"
#include "parser.h"
#include "scanner.h"
#include "errors.h"
#include "prec_table.h"
#include "bu_analysis.h"
#include "utils.h"
#include "code_gen.h"
#include "syntax_rules.h"


int main() {
    Parser_t parser;
    Error err;
    parser_init(&parser);

    err = parser_rule_stmtMainSeq(&parser);
    
    parser_dtor(&parser);
    return err;
}