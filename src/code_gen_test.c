#include "code_gen.h"
#include "symtable.h"
#include <stdlib.h>
#include <stdio.h>
int main() {
    /*
    {
    char *a = 2 + 3 != 69 ? "yeeey" : "boo";
    prinf("%s\n",a);
    }
    */
    symtable_t table;
    table_init(&table);
    gen_prog();
    gen_local_scope(&table);
    int if_ln = get_cond_label();
    symtable_entry_t *entry;
    table_insert(&table,"a",&entry);

    gen_def_var("a",false,TOKEN_DT_STRING); //char *a;

    // 2 + 3 != 69
    litValue value = {.i = 2};
    push_literal(value,INT_LIT);
    value.i = 3;
    push_literal(value,INT_LIT);
    gen_expr_binop('+');
    value.i = 69;
    push_literal(value, INT_LIT);
    gen_cond(NEQ);
    //{
    gen_local_scope(&table);
    gen_cnd_jump("IF_ELSE",if_ln);
    //palti
    value.str = "yeeey";
    push_literal(value, STRING_LIT);
    gen_assignment("a",false);
    gen_jmp(if_ln);
    //neplati
    gen_cond_else_label(if_ln);
    value.str = "boo";
    push_literal(value, STRING_LIT);
    gen_assignment("a",false);
    gen_cond_end_label(if_ln);
    gen_drop_local_scope(&table);
    //}
    //printf("%s\n",a);
    gen_write_var("a",false);
    value.str = "\\010";
    gen_write_lit(value,STRING_LIT);

    table_dispose(&table);
    return 0;
}