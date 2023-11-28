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
    gen_push_lit(value,INT_LIT);
    value.i = 3;
    gen_push_lit(value,INT_LIT);
    gen_expr_binop('+');
    value.i = 69;
    gen_push_lit(value, INT_LIT);
    gen_cond(NEQ);
    //{
    gen_local_scope(&table);
    gen_cnd_jump("IF_ELSE",if_ln);
    //palti
    value.str = "yeeey";
    gen_push_lit(value, STRING_LIT);
    gen_assignment("a",false);
    gen_jmp("IF_END",if_ln);
    //neplati
    gen_cond_else_label(if_ln);
    value.str = "boo";
    gen_push_lit(value, STRING_LIT);
    gen_assignment("a",false);
    gen_end_label("IF",if_ln);
    gen_drop_local_scope(&table);
    //}
    //printf("%s\n",a);
    gen_write_var("a",false);
    value.str = "\\010";
    gen_write_lit(value,STRING_LIT);
    gen_prog_end(0);
    table_dispose(&table);
}


void test_fun() {
    symtable_t table;
    table_init(&table);
    param_t **params = calloc(3,sizeof(param_t*));
    param_t *param1 = calloc(1,sizeof(param_t));
    param1->id = "x";
    params[0] = param1;
    param_t *param2 = calloc(1,sizeof(param_t));
    param2->id = "y";
    params[1] = param2;
    table_function_insert(&table,"func1",params,RET_INT);
    gen_prog();

    gen_def_var("a",true,TOKEN_DT_INT);
    litValue value = {.i = 3};
    gen_push_lit(value,INT_LIT);
    gen_assignment("a",true);
    gen_def_var("b",true,TOKEN_DT_INT);
    value.i = 7;
    gen_push_lit(value,INT_LIT);
    gen_assignment("b",true);
    gen_local_scope(&table);
    int ln = get_loop_label();
    gen_loop_label(ln);
    gen_push_var("a",true);
    value.i = 1000;
    gen_push_lit(value,INT_LIT);
    gen_cond(LTE);
    gen_cnd_jump("LOOP_END",ln);
    gen_func_pre_call(params);
    add_var_arg("x","a",true);
    add_var_arg("y","b",true);
    gen_func_call("func1");
    gen_assignment("a",true);
    gen_write_var("a",true);
    value.str = "\\010";
    gen_write_lit(value,STRING_LIT);
    gen_jmp("LOOP",ln);
    gen_end_label("LOOP",ln);
    gen_drop_local_scope(&table);
    gen_prog_end(0);


    gen_func_def(params,"func1");
    value.i = 1;
    gen_def_var("a", false, TOKEN_DT_INT);
    gen_def_var("b", false, TOKEN_DT_STRING);
    gen_push_var("x",false);
    gen_push_var("y",false);
    gen_expr_binop('+');
    gen_func_return();

    
    table_dispose(&table);
}

int main() {
    /*bud jeden alebo druhy test ak to budete spustat*/
//    test_condition();
    test_fun();
    return 0;
}