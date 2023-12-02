#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../code_gen.h"
#include "../symtable.h"

const int int_const1 = 15;
const int int_const2 = 12;
const int int_const3 = 4;

const double dub_const1 = 15.0;
const double dub_const2 = 12.0;
const double dub_const3 = 4.0;

char str_const1[11] = "Hello";
char str_const2[11] = "World";


#define TEST(name)                      \
    symtable_t table;                   \
    litValue value;                     \
    param_t *param;                     \
    symtable_entry_t *entry;            \
    ParamBufferT pbuf;                  \
    int cnt_label = get_cont_label();   \
    param_buffer_init(&pbuf);           \
    table_init(&table);                 \
    inset_test_label(name);             \
    gen_cont_label(cnt_label);          \
    gen_local_scope(&table);            \
    table_add_scope(&table);            \
    char *a = lit2ptr("a");             \
    table_insert(&table,a,&entry);      \
    gen_def_var(a,false,TOKEN_DT_INT);  \

#define END_TEST                    \
    table_remove_scope(&table);     \
    gen_drop_local_scope(&table);   \
    table_dispose(&table);          \
    param_buffer_detor(&pbuf);      \
    jump_cont_label(cnt_label+1);   \


#define OUTPUT_VAR(var)                                 \
    param = param_create(NULL, NULL,TOKEN_VAR);         \
    value.str = var;                                    \
    param_value_init(&table,param,value,TOKEN_VAR);     \
    gen_write_arg(param);                               \
    value.str = "\\010";                                \
    param->type = TOKEN_STRING;                         \
    param_value_init(&table,param,value,TOKEN_STRING);  \
    gen_write_arg(param);                               \
    free(param);                                        \

#define PUSH_INT_LIT(val)               \
    value.i = val;                      \
    gen_push_lit(value,TOKEN_INTEGER);  \


#define PUSH_DUB_LIT(val)               \
    value.d = val;                      \
    gen_push_lit(value,TOKEN_DOUBLE);   \


#define PUSH_STR_LIT(val)               \
    value.str = val;                      \
    gen_push_lit(value,TOKEN_STRING);   \

#define PUSH_NIL                     \
    gen_push_lit(value,TOKEN_NIL);   \


void inset_test_label(char *test_name) {
    printf("\n##########test_%s########\n\n",test_name);
}

void test_add_int() {
    TEST("add_int")
    value.i = int_const1;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('+');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_sub_int() {
    TEST("sub_int")
    value.i = int_const2;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = int_const1;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('-');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_mult_int() {
    TEST("mult_int")
    value.i = int_const1;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('*');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_div_int() {
    TEST("div_int")
    value.i = int_const2;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = int_const3;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('\\');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_dubq_int() {
    TEST("dubq_int");
    value.i = int_const2;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_push_lit(value,TOKEN_NIL);
    gen_expr_binop('?');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    gen_push_lit(value, TOKEN_NIL);
    value.i = int_const3;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_expr_binop('?');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_add_double() {
    TEST("add_double")
    value.d = dub_const1;
    gen_push_lit(value,TOKEN_DOUBLE);
    value.d = dub_const2;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_expr_binop('+');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_sub_double() {
    TEST("sub_double")
    value.d = dub_const2;
    gen_push_lit(value,TOKEN_DOUBLE);
    value.d = dub_const1;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_expr_binop('-');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_mult_double() {
    TEST("mult_double")
    value.d = dub_const2;
    gen_push_lit(value,TOKEN_DOUBLE);
    value.d = dub_const1;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_expr_binop('*');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_div_double() {
    TEST("div_double")
    value.d = dub_const2;
    gen_push_lit(value,TOKEN_DOUBLE);
    value.d = dub_const3;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_expr_binop('/');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_dubq_double() {
    TEST("dubq_double")
    value.d = dub_const2;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_push_lit(value,TOKEN_NIL);
    gen_expr_binop('?');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    gen_push_lit(value, TOKEN_NIL);
    value.d = dub_const3;
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_expr_binop('?');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_concat_str() {
    TEST("concat_str")
    value.str = str_const1;
    gen_push_lit(value,TOKEN_STRING);
    value.str = str_const2;
    gen_push_lit(value,TOKEN_STRING);
    gen_expr_binop('|');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = lit2ptr("World");
    gen_push_lit(value,TOKEN_STRING);
    free(value.str);
    value.str = lit2ptr("Hello");
    gen_push_lit(value,TOKEN_STRING);
    free(value.str);
    gen_expr_binop('|');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}


void test_builtin_int2double() {
    TEST("builtin_int2double")
    value.i = int_const2;
    gen_push_lit(value,TOKEN_INTEGER);   
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    table_search_global(&table,"Int2Double",&entry);
    value.str = a;
    param_value_init(&table,entry->params[0],value,TOKEN_VAR);
    gen_func_call("Int2Double",entry);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}


void test_builtin_double2int() {
    TEST("builtin_double2int")
    value.d = dub_const2;
    gen_push_lit(value,TOKEN_DOUBLE);   
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    table_search_global(&table,"Double2Int",&entry);
    value.str = a;
    param_value_init(&table,entry->params[0],value,TOKEN_VAR);
    gen_func_call("Double2Int",entry);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_builtint_substring() {
    TEST("builtin_substring")
    value.str = "HelloWorld";
    gen_push_lit(value,TOKEN_STRING);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = "HelloWolrd";
    table_search_global(&table,"substring",&entry);
    param_value_init(&table,entry->params[0],value,TOKEN_STRING);
    value.i = 2;
    param_value_init(&table,entry->params[1],value,TOKEN_INTEGER);
    value.i = 5;
    param_value_init(&table,entry->params[2],value,TOKEN_INTEGER);
    gen_func_call("substring",entry);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_builtin_chr() {
    TEST("builtin_chr")
    value.i = 50;
    table_search_global(&table,"chr",&entry);
    param_value_init(&table,entry->params[0],value,TOKEN_INTEGER);
    gen_func_call("chr",entry);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_builtin_ord() {
    TEST("builtin_ord")
    value.str = "25699";
    table_search_global(&table,"ord",&entry);
    param_value_init(&table,entry->params[0],value,TOKEN_STRING);
    gen_func_call("ord",entry);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_lt() {
    TEST("lt")
    value.i = int_const1;
    gen_push_lit(value, TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_cond(LT);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.d = dub_const1;
    gen_push_lit(value, TOKEN_DOUBLE);
    value.d = dub_const2;
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(LT);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = "Hello";
    gen_push_lit(value, TOKEN_DOUBLE);
    value.str = "World";
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(LT);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_lte() {
    TEST("lte")
    value.i = int_const1;
    gen_push_lit(value, TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_cond(LTE);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.d = dub_const1;
    gen_push_lit(value, TOKEN_DOUBLE);
    value.d = dub_const2;
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(LTE);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = "Hello";
    gen_push_lit(value, TOKEN_DOUBLE);
    value.str = "World";
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(LTE);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}


void test_gt() {
    TEST("gt")
    value.i = int_const1;
    gen_push_lit(value, TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_cond(GT);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.d = dub_const1;
    gen_push_lit(value, TOKEN_DOUBLE);
    value.d = dub_const2;
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(GT);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = "Hello";
    gen_push_lit(value, TOKEN_DOUBLE);
    value.str = "World";
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(GT);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}


void test_gte() {
    TEST("gte")
    value.i = int_const1;
    gen_push_lit(value, TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_cond(GTE);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.d = dub_const1;
    gen_push_lit(value, TOKEN_DOUBLE);
    value.d = dub_const2;
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(GTE);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = "Hello";
    gen_push_lit(value, TOKEN_DOUBLE);
    value.str = "World";
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(GTE);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}


void test_eq() {
    TEST("eq")
    value.i = int_const1;
    gen_push_lit(value, TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_cond(EQ);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.d = dub_const1;
    gen_push_lit(value, TOKEN_DOUBLE);
    value.d = dub_const2;
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(EQ);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = "Hello";
    gen_push_lit(value, TOKEN_DOUBLE);
    value.str = "World";
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(EQ);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}


void test_neq() {
    TEST("neq")
    value.i = int_const1;
    gen_push_lit(value, TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_cond(NEQ);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.d = dub_const1;
    gen_push_lit(value, TOKEN_DOUBLE);
    value.d = dub_const2;
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(NEQ);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = "Hello";
    gen_push_lit(value, TOKEN_DOUBLE);
    value.str = "World";
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_cond(NEQ);
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

/*
    a :Int = 0
    b = 69
    {
        a :Int = 1
        {
            a :Int = 2
            {
                a :Int = 3
                    ...
                {
                    a : Int = 14
                }
            }
        }
    }
*/

void test_nested_scopes() {
    TEST("nested_scopes")
    printf("MOVE LF@a int@0\n");
    OUTPUT_VAR(a)
    printf("DEFVAR LF@b\n");
    printf("MOVE LF@b int@69\n");
    char *b = lit2ptr("b");
    table_insert(&table,b,&entry);
    OUTPUT_VAR(b)
    for (int i = 1; i < 15; i++) {
        table_add_scope(&table);
        gen_local_scope(&table);
        char *a = lit2ptr("a");
        table_insert(&table,a,&entry);
        printf("MOVE LF@a int@%d\n",i);
        OUTPUT_VAR(a)
    }
    OUTPUT_VAR(b)
    for (int i = 15; i > 1; i--) {
        gen_drop_local_scope(&table);
        table_remove_scope(&table);
        OUTPUT_VAR(a)
    }
    OUTPUT_VAR(a)                                     
    OUTPUT_VAR(b)
    END_TEST
}

/*
    a = 15+12*4
*/
void test_expr() {
    TEST("expr_15+12*4")
    value.i = int_const1;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = int_const2;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = int_const3;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('*');
    gen_expr_binop('+');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}
/*

a = custom_funcion(a,129)

custom_funcion(_ x:Int, _ y:Int) -> Int {
    return x + y
}
*/
void test_custom_funcion() {
    TEST("custom_funcion")
    /*funcion insert*/
    param = param_from_lit_create("x","_",TOKEN_DT_INT);
    table_insert_param(&pbuf,param);
    param = param_from_lit_create("y","_",TOKEN_DT_INT);
    table_insert_param(&pbuf,param);
    table_function_insert(&table,lit2ptr("custom_func"),param_buffer_export(&pbuf),TOKEN_INTEGER);

    table_search_global(&table,"custom_func",&entry);
    entry->params[0]->var = 1;
    entry->params[0]->value.str = "a";
    entry->params[1]->value.i = 144-15;
    printf("MOVE LF@a int@15\n");
    table_add_scope(&table);
    symtable_entry_t *tmp;
    table_insert(&table,lit2ptr("x"),&tmp);
    table_insert(&table,lit2ptr("y"),&tmp);
    gen_func_call("custom_func",entry);

    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    
    cnt_label = get_cont_label();
    jump_cont_label(cnt_label);
    gen_func_def("custom_func");
    gen_push_var("x",is_global(&table,"x"));
    gen_push_var("y",is_global(&table,"y"));
    gen_expr_binop('+');
    table_remove_scope(&table);
    gen_func_return();
    gen_cont_label(cnt_label);
    END_TEST
}
/*
a :Int = 0
if (3 > 12){
    a = 3
} else {
    a = 12
}
*/
void test_if_else() {
    TEST("if_else")
    OUTPUT_VAR(a)
    int ln = get_cond_label();
    PUSH_INT_LIT(int_const3)
    PUSH_INT_LIT(int_const2)
    gen_cond(GT);
    gen_cnd_jump(ELSE_L,ln);
    table_add_scope(&table);
    gen_local_scope(&table);
    PUSH_INT_LIT(int_const3)
    table_insert(&table,lit2ptr("b"),&entry);
    PUSH_STR_LIT("IF_branch")
    gen_assignment("b",is_global(&table,"b"));
    OUTPUT_VAR("b")
    table_search(&table,a,&entry);
    entry->modified = true;
    gen_assignment(a,is_global(&table,a));
    gen_drop_local_scope(&table);
    table_remove_scope(&table);

    gen_jmp(IF_END_L,ln);
    gen_cond_else_label(ln);
    table_add_scope(&table);
    gen_local_scope(&table);
    PUSH_INT_LIT(int_const2)
    table_insert(&table,lit2ptr("b"),&entry);
    PUSH_STR_LIT("ELSE_BRANCH")
    gen_def_var("b",is_global(&table,"b"),TOKEN_INTEGER);
    gen_assignment("b",is_global(&table,"b"));
    OUTPUT_VAR("b")
    table_search(&table,a,&entry);
    entry->modified = true;
    gen_assignment(a,is_global(&table,a));
    gen_drop_local_scope(&table);
    table_remove_scope(&table);
    gen_end_label(IF_L,ln);
    OUTPUT_VAR(a)
    END_TEST
}

/*
{
    a :Int = 0

    while(a < 100) {
        if (a <= 4) {
            a = a + 1
        }
        a = a + 3
    }
}
*/
void test_loop() {
    TEST("loop")
    OUTPUT_VAR(a)
    int lpn = get_loop_label();
    table_add_scope(&table);
    gen_local_scope(&table);
    gen_loop_label(lpn);
    gen_push_var(a,is_global(&table,a));
    PUSH_INT_LIT(100);
    gen_cond(LT);
    int cndn = get_cond_label();
    gen_cnd_jump(LOOP_END_L,lpn);
    gen_push_var(a,is_global(&table,a));
    PUSH_INT_LIT(4)
    gen_cond(LTE);
    gen_cnd_jump(ELSE_L,cndn);
    table_add_scope(&table);
    gen_local_scope(&table);
    gen_push_var(a,is_global(&table,a));
    PUSH_INT_LIT(1)
    gen_expr_binop('+');
    gen_assignment(a,is_global(&table,a));
    table_search(&table,a,&entry);
    entry->modified = 1;
    gen_drop_local_scope(&table);
    table_remove_scope(&table);
    gen_jmp(IF_END_L,cndn);
    gen_cond_else_label(cndn);
    table_add_scope(&table);
    gen_local_scope(&table);
    gen_drop_local_scope(&table);
    table_remove_scope(&table);
    gen_end_label(IF_L,cndn);
    gen_push_var(a,is_global(&table,a));
    PUSH_INT_LIT(4)
    gen_expr_binop('+');
    gen_assignment(a,is_global(&table,a));
    table_search(&table,a,&entry);
    entry->modified = 1;
    gen_jmp(LOOP_L,lpn);
    gen_end_label(LOOP_L,lpn);
    gen_drop_local_scope(&table);
    table_remove_scope(&table);

    OUTPUT_VAR(a)
    END_TEST

}



int main() {
    gen_prog();
    test_add_int();
    test_sub_int();
    test_mult_int();
    test_div_int();
    test_dubq_int();
    test_add_double();
    test_sub_double();
    test_mult_double();
    test_div_double();
    test_dubq_double();
    test_concat_str();
    test_builtin_int2double();
    test_builtin_double2int();
    test_builtint_substring();
    test_builtin_chr();
    test_builtin_ord();
    test_lt();
    test_lte();
    test_gt();
    test_gte();
    test_eq();
    test_neq();
    test_nested_scopes();
    test_expr();
    test_custom_funcion();
    test_if_else();
    test_loop();
    gen_prog_end(0);

    return 0;
}