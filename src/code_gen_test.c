#include <stdio.h>
#include <stdlib.h>
#include "code_gen.h"
#include "symtable.h"

void test_condition() {
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
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = 3;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('+');
    value.i = 69;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_cond(NEQ);
    //{
    gen_cnd_jump("IF_ELSE",if_ln);
    //palti
    value.str = "yeeey";
    gen_push_lit(value, TOKEN_STRING);
    gen_assignment("a",false);
    gen_jmp("IF_END",if_ln);
    //neplati
    gen_cond_else_label(if_ln);
    value.str = "boo";
    gen_push_lit(value, TOKEN_STRING);
    gen_assignment("a",false);
    gen_end_label("IF",if_ln);
    //}
    //printf("%s\n",a);
    gen_prog_end(0);
    table_dispose(&table);
}


void test_fun() {
    symtable_t table;
    table_init(&table);
    param_t **params = calloc(3,sizeof(param_t*));
    param_t *param1 = calloc(1,sizeof(param_t));
    param1->id = lit2ptr("x");
    param1->global = true;
    param1->type = TOKEN_VAR;
    param1->value.str = "a";
    params[0] = param1;
    param_t *param2 = calloc(1,sizeof(param_t));
    param2->id = lit2ptr("y");
    param2->global = true;
    param2->type = TOKEN_VAR;
    param2->value.str = "b";
    params[1] = param2;
    table_function_insert(&table,lit2ptr("func1"),params,TOKEN_INTEGER);
    symtable_entry_t *entry;
    table_search_global(&table,"func1",&entry);
    gen_prog();

    gen_def_var("a",true,TOKEN_STRING);
    litValue value = {.str = "Hello"};
    gen_push_lit(value,TOKEN_STRING);
    gen_assignment("a",true);
    gen_def_var("b",true,TOKEN_STRING);
    value.str = "Wolrd";
    gen_push_lit(value,TOKEN_STRING);
    gen_assignment("b",true);
    gen_def_var("c",true,TOKEN_DT_INT);
    // gen_local_scope(&table);
    int ln = get_loop_label();
    gen_loop_label(ln);
    gen_push_var("a",true);
    gen_push_var("c",true);
    gen_string_op('l');
    value.i = 5;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_cond(LTE);
    gen_cnd_jump("LOOP_END",ln);
    gen_func_call("func1",entry);
    gen_assignment("a",true);
    entry->params[0]->global = true;
    entry->params[0]->value.str = "a";
    entry->params[0]->type = TOKEN_VAR;
    gen_write_arg(entry->params[0]);
    entry->params[0]->type = TOKEN_STRING;
    entry->params[0]->value.str = "\\010"; 
    gen_write_arg(entry->params[0]);
    gen_jmp("LOOP",ln);
    gen_end_label("LOOP",ln);
    // gen_drop_local_scope(&table);

    gen_prog_end(0);

    gen_func_def("func1");
    value.i = 1;
    gen_def_var("a", false, TOKEN_DT_INT);
    gen_def_var("b", false, TOKEN_DT_STRING);
    gen_push_var("b",false);
    gen_push_var("x",false);
    gen_push_var("y",false);
    gen_string_op('|');
    gen_func_return();

    
    table_dispose(&table);
}

int main() {
    /*bud jeden alebo druhy test ak to budete spustat*/
//    test_condition();
    test_fun();
    return 0;
}