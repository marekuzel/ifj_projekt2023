#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_gen.h"
#include "symtable.h"

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

#define PUSH_INT_LIT(val)               \
    value.i = val;                      \
    gen_push_lit(value,TOKEN_INTEGER);  \


#define PUSH_DUB_LIT(val)               \
    value.d = val;                      \
    gen_push_lit(value,TOKEN_DOUBLE);   \


#define PUSH_STR_LIT(val)               \
    value.i = val;                      \
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
    gen_expr_binop('/');
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
    gen_prog_end(0);

    return 0;
}