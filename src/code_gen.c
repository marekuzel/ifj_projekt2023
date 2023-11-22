#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "code_gen.h"
#include "symtable.h"

void gen_assignment(char *identifier, bool global) {
    if (global) {
        printf("POPS GF@%s\n",identifier);
    } else {
        printf("POPS LF@%s\n",identifier);
    }
    printf("CLEARS\n");
}

void gen_def_var(char *id, bool global) {
    if (global) {
        printf("DEFVAR GF@%s\n",id);
    } else {
        printf("DEFVAR LF@%s\n",id);
    }
}

void push_var(char *id, bool global){

    if (global){
        printf("PUSHS GF@%s\n",id);
    } else {
        printf("PUSHS LF@%s\n",id);
    }
}


void push_literal(litValue value, lit_type_t type) {
    switch (type)
    {
    case INT_LIT:
        printf("PUSHS int@%d\n",value.i);
        break;
    
    case DOUBLE_LIT:
        printf("PUSHS float@%lf\n",value.d);
        break;

    case STRING_LIT:
        printf("PUSH string@%s\n",value.str);
        break;

    case NILL:
        printf("PUSHS nil@nil\n");
        break;

    default:
        break;
    }
}


void clear_expr_stack() {
    printf("CLEARS\n");
}

void gen_expr_binop(const char operator) {

    int label_num;
    int var1;
    int var2;

    switch (operator) {
        
    case '+':
        printf("ADDS\n");
        break;

    case '-':
        printf("SUBS\n");
        break;

    case '*':
        printf("MULTS\n");
        break;

    case '/':
        printf("IDIVS\n");
        break;

    case '\\':
        printf("DIVS\n");
        break;

    case '?':

        label_num = get_cond_label();
        var1 = get_new_var();
        var2 = get_new_var();

        printf("DEFVAR LF@var%d\n",var1);
        printf("DEFVAR LF@var%d\n",var2);

        printf("POPS LF@var%d\n",var2); //b
        printf("POPS LF@var%d\n",var1); //a


        printf("PUSHS LF@var%d",var1);
        printf("PUSHS nil@nil\n");
        gen_cond(EQ);

        
        gen_cnd_jump("IF_ELSE", label_num);

        printf("PUSHS LF@var%d\n",var2);
        printf("JUMP IF_END%d\n",label_num);

        printf("LABEL IF_ELSE%d\n",label_num);
        printf("PUSHS LF@var%d\n",var1);

        printf("LABEL IF_END\n");
        break;

    default:
        break;
    }
}

void gen_expr_conv(conv_type_t conversion_type) {

    switch (conversion_type){

    case FI:
        printf("FLOAT2INTS\n");
        break;

    case IF:
        printf("INT2FLOATS\n");
        break;

    case IC:
        printf("INT2CAHRS\n");
        break;

    case CI:
        printf("SRING2INTS");
        break;
    default:
        break;
    }
}

void gen_cond(rel_op_t relation_operator) {
    
    switch (relation_operator) {

    case LT:
        printf("LTS\n");
        break;

    case LTE:
        printf("GTS\n");
        printf("NOTS\n");
        break;

    case GT:
        printf("GT\n");
        break;

    case GTE:
        printf("LTS\n");
        printf("NOTS\n");
        break;

    case EQ:
        printf("EQS\n");
        break;

    case NEQ:
        printf("EQS\n");
        printf("NOTS");
    default:
        break;
    }
}

int get_loop_label_num() {
    static int loop_label_num = 0;
    loop_label_num++;
    return loop_label_num;
}


int get_cond_label() {
    static int cond_label_num = 0;
    cond_label_num ++;
    return cond_label_num;
}

void gen_loop_label(int loop_label_num) {
    printf("LABEL Loop%d\n",loop_label_num);
}

void gen_cond_else_label(int cond_label_num) {
    printf("LABEL IF_ELSE%d\n",cond_label_num);
}

void gen_cond_end_label(int cond_label_num) {
    printf("LABEL IF_END%d\n",cond_label_num);
}

void gen_cnd_jump(char *dest_type, int dest_number) {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNQS %s%d\n",dest_type, dest_number);
}

void gen_func_def(char *name) {
    printf("LABEL %s\n",name);
    printf("PUSHFRAME\n");
}

void gen_func_return() {
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void gen_local_scope(symtable_t *table) {
    printf("CREATEFRAME\n");
    table_copy_local(table);
    printf("PUSHFRAME");
}


void gen_func_pre_call(param_t **params) {
    printf("CREATEFRAME\n");
    for (int param_idx = 0; params[param_idx] != NULL; param_idx++) {
        printf("DEFVAR TF@%s",params[param_idx]->id);
    }
}

void add_func_arg(char *name) {
    printf("POPS TF@n%s\n",name);
    clear_expr_stack();
}

void gen_func_call(char *name) {
    printf("CALL %s\n",name);
}

// void gen_cond_branch(ast_node_t* ast) {
//     static int cond_label_num = 0;
//     cond_label_num++;

//     gen_cond(ast->left,"If", cond_label_num);
//     printf("LABEL If%d\n", cond_label_num);
//     gen_statements(ast->next);
//     printf("JUMP End_if%d\n",cond_label_num);
    
//     if (ast->has_else) {
//         gen_statements(ast->right);
//     }

//     printf("LABEL End_if%d\n", cond_label_num);
    
// }

int get_new_var() {
    static int var_number = 0;
    var_number++;
    return var_number;
}

void gen_read(char *identifier, bool global, const char *type) {
    if (global) {
        printf("READ GF@%s %s\n",identifier, type);
    } else {
        printf("READ LF@%s %s\n",identifier, type);
    }
}

void gen_write(char *string) {
    printf("WRITE %s\n",string);
}

void gen_string_op(const char operator) {

    int var_number1 = get_new_var();
    int var_number2 = get_new_var();
    int var_number3 = get_new_var();

    switch (operator) {
    case 'l':
        printf("DEFVAR LF@var%d\n",var_number1);
        printf("DEFVAR LF@var%d\n",var_number2);

        printf("POPS LF@var%d\n",var_number2);  
        printf("POPS LF@var%d\n",var_number1);

        printf("STRLEN LF@var%d LF@var%d\n",var_number1,var_number2);
        printf("PUSHS LF@var%d\n",var_number1);
        break;
    case '|':
        printf("DEFVAR LF@var%d\n",var_number1);
        printf("DEFVAR LF@var%d\n",var_number2);
        printf("DEFVAR LF@var%d\n",var_number3);

        printf("POPS LF@var%d\n",var_number3);  
        printf("POPS LF@var%d\n",var_number2);  
        printf("POPS LF@var%d\n",var_number1);

        printf("CONCAT LF@var%d LF@var%d LF@var%d\n",var_number1, var_number2, var_number3);
        printf("PUSHS LF@var%d\n",var_number3);
        break;
    case 'g':
        printf("DEFVAR LF@var%d\n",var_number1);
        printf("DEFVAR LF@var%d\n",var_number2);
        printf("DEFVAR LF@var%d\n",var_number3);

        printf("POPS LF@var%d\n",var_number3);  
        printf("POPS LF@var%d\n",var_number2);  
        printf("POPS LF@var%d\n",var_number1);

        printf("GETCHAR LF@var%d LF@var%d LF@var%d\n",var_number1, var_number2, var_number3);
        printf("PUSHS LF@var%d\n",var_number3);
        break;
    
    case 's':
        printf("DEFVAR LF@var%d\n",var_number1);
        printf("DEFVAR LF@var%d\n",var_number2);
        printf("DEFVAR LF@var%d\n",var_number3);

        printf("POPS LF@var%d\n",var_number3);  
        printf("POPS LF@var%d\n",var_number2);  
        printf("POPS LF@var%d\n",var_number1);

        printf("SETCHAR LF@var%d LF@var%d LF@var%d\n",var_number1, var_number2, var_number3);
        printf("PUSHS LF@var%d\n",var_number3);
    default:
        break;
    }
}

void var_copy(awl_t *awl) {
    printf("DEFVAR TF@%s\n",awl->key);
    printf("MOVE LF@%s TF@%s\n",awl->key,awl->key);
}