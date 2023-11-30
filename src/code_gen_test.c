#include <stdio.h>
#include <stdlib.h>
#include "code_gen.h"
#include "symtable.h"

#define TEST(name)                      \
    symtable_t table;                   \
    litValue value;                     \
    param_t *param;                     \
    symtable_entry_t *entry;            \
    table_init(&table);                 \
    inset_test_label(name);             \
    gen_local_scope(&table);            \
    table_add_scope(&table);            \
    char *a = lit2ptr("a");             \
    table_insert(&table,a,&entry);      \
    gen_def_var(a,false,TOKEN_DT_INT);  \

#define END_TEST                    \
    gen_drop_local_scope(&table);   \
    table_remove_scope(&table);     \
    table_dispose(&table);          \


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

void inset_test_label(char *test_name) {
    printf("\n##########test_%s########\n\n",test_name);
}

void test_add_int() {
    TEST("add_int")
    value.i = 15;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = 12;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('+');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_sub_int() {
    TEST("sub_int")
    value.i = 12;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = 15;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('-');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_mult_int() {
    TEST("mult_int")
    value.i = 12;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = 15;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('*');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_div_int() {
    TEST("div_int")
    value.i = 12;
    gen_push_lit(value,TOKEN_INTEGER);
    value.i = 4;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_expr_binop('/');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_dubq_int() {
    TEST("dubq_int");
    value.i = 12;
    gen_push_lit(value,TOKEN_INTEGER);
    gen_push_lit(value,TOKEN_NIL);
    gen_expr_binop('?');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    gen_push_lit(value, TOKEN_NIL);
    value.i = 4;
    gen_push_lit(value, TOKEN_INTEGER);
    gen_expr_binop('?');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_add_double() {
    TEST("add_double")
    value.d = 15.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    value.d = 12.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_expr_binop('+');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_sub_double() {
    TEST("sub_double")
    value.d = 12.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    value.d = 15.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_expr_binop('-');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_mult_double() {
    TEST("mult_double")
    value.d = 12.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    value.d = 15.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_expr_binop('*');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_div_double() {
    TEST("div_double")
    value.d = 12.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    value.d = 4.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_expr_binop('/');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_dubq_double() {
    TEST("dubq_double")
    value.d = 12.0;
    gen_push_lit(value,TOKEN_DOUBLE);
    gen_push_lit(value,TOKEN_NIL);
    gen_expr_binop('?');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    gen_push_lit(value, TOKEN_NIL);
    value.d = 4.0;
    gen_push_lit(value, TOKEN_DOUBLE);
    gen_expr_binop('?');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}

void test_concat_str() {
    TEST("concat_str")
    value.str = "Hello";
    gen_push_lit(value,TOKEN_STRING);
    value.str = "World";
    gen_push_lit(value,TOKEN_STRING);
    gen_string_op('|');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    value.str = lit2ptr("World");
    gen_push_lit(value,TOKEN_STRING);
    free(value.str);
    value.str = lit2ptr("Hello");
    gen_push_lit(value,TOKEN_STRING);
    free(value.str);
    gen_string_op('|');
    gen_assignment(a,is_global(&table,a));
    OUTPUT_VAR(a)
    END_TEST
}


void test_builtin_int2double() {
    TEST("builtin_int2double")
    value.i = 12;
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
    value.d = 12.0;
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
    value.str = "HelloWolrd";
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
    gen_prog_end(0);
    return 0;
}