#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "code_gen.h"
#include "symtable.h"


void gen_prog() {
    printf(".IFJcode23\n");
}
void gen_prog_end(int exit_code) {
    printf("EXIT int@%d\n",exit_code);
}

void gen_assignment(char *identifier, bool global) {
    char *scope = global ? "GF" : "LF";
    printf("POPS %s@%s\n",scope,identifier);
    //printf("CLEARS\n");
}

void gen_def_var(char *id, bool global, TokenType type) {
    char *scope = global ? "GF" : "LF";

    printf("DEFVAR %s@%s\n",scope,id);
    switch (type) {
    case TOKEN_DT_INT:
        printf("MOVE %s@%s int@0\n",scope, id);
        break;

    case TOKEN_DT_DOUBLE:
        printf("MOVE %s@%s float@%a\n",scope, id, 0.0);
        break;
    case TOKEN_DT_STRING:
        printf("MOVE %s@%s string@\n",scope, id);
        break;
    default:
        printf("MOVE %s@%s nil@nil\n",scope, id);
        break;
    }
}

void push_var(char *id, bool global){
    const char *scope = global ? "GF" : "LF";
    printf("PUSHS %s@%s\n",scope,id);
}


void push_literal(litValue value, lit_type_t type) {
    switch (type) {
    case INT_LIT:
        printf("PUSHS int@%d\n",value.i);
        break;
    
    case DOUBLE_LIT:
        printf("PUSHS float@%a\n",value.d);
        break;

    case STRING_LIT:
        printf("PUSHS string@%s\n",value.str);
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

void gen_var_copy(awl_t *awl) {
    if (awl->value->redeclared || awl->value->type == TOKEN_FUNC)
        return;

    printf("DEFVAR TF@%s\n",awl->key);
    printf("MOVE TF@%s LF@%s\n",awl->key,awl->key);
}

void gen_var_val_move(awl_t *awl) {
    if (awl->value->redeclared || awl->value->type == TOKEN_FUNC)
        return;

    printf("MOVE LF@%s TF@%s\n",awl->key,awl->key);
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
        printf("MULS\n");
        break;

    case '/':
        printf("IDIVS\n");
        break;

    case '\\':
        printf("DIVS\n");
        break;

    case '?':
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        label_num = get_cond_label();
        var1 = get_new_var();
        var2 = get_new_var();

        printf("DEFVAR LF@var%d\n",var1);
        printf("DEFVAR LF@var%d\n",var2);

        printf("POPS LF@var%d\n",var2); //b
        printf("POPS LF@var%d\n",var1); //a


        printf("PUSHS LF@var%d\n",var1);
        printf("PUSHS nil@nil\n");
        gen_cond(EQ);

        
        gen_cnd_jump("IF_ELSE", label_num);

        printf("PUSHS LF@var%d\n",var2);
        printf("JUMP IF_END%d\n",label_num);

        printf("LABEL IF_ELSE%d\n",label_num);
        printf("PUSHS LF@var%d\n",var1);

        printf("LABEL IF_END%d\n",label_num);

        printf("POPFRAME\n");
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
        printf("SRING2INTS\n");
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
        printf("GTS\n");
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
        printf("NOTS\n");
    default:
        break;
    }
}

void gen_string_op(const char operator) {

    int dest = get_new_var();
    int op1 = get_new_var();
    int op2 = get_new_var();

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    switch (operator) {
    case 'l':
        printf("DEFVAR LF@var%d\n",dest);
        printf("DEFVAR LF@var%d\n",op1);

        printf("POPS LF@var%d\n",op1);  
        printf("POPS LF@var%d\n",dest);

        printf("STRLEN LF@var%d LF@var%d\n",dest,op1);
        printf("PUSHS LF@var%d\n",dest);
        break;
    case '|':
        printf("DEFVAR LF@var%d\n",dest);
        printf("DEFVAR LF@var%d\n",op1);
        printf("DEFVAR LF@var%d\n",op2);

        printf("POPS LF@var%d\n",op2);  
        printf("POPS LF@var%d\n",op1);  
        printf("POPS LF@var%d\n",dest);

        printf("CONCAT LF@var%d LF@var%d LF@var%d\n",dest, op1, op2);
        printf("PUSHS LF@var%d\n",dest);
        break;
    case 'g':
        printf("DEFVAR LF@var%d\n",dest);
        printf("DEFVAR LF@var%d\n",op1);
        printf("DEFVAR LF@var%d\n",op2);

        printf("POPS LF@var%d\n",op2);  
        printf("POPS LF@var%d\n",op1);  
        printf("POPS LF@var%d\n",dest);

        printf("GETCHAR LF@var%d LF@var%d LF@var%d\n",dest, op1, op2);
        printf("PUSHS LF@var%d\n",dest);
        break;
    
    case 's':
        printf("DEFVAR LF@var%d\n",dest);
        printf("DEFVAR LF@var%d\n",op1);
        printf("DEFVAR LF@var%d\n",op2);

        printf("POPS LF@var%d\n",op2);  
        printf("POPS LF@var%d\n",op1);  
        printf("POPS LF@var%d\n",dest);

        printf("SETCHAR LF@var%d LF@var%d LF@var%d\n",dest, op1, op2);
        printf("PUSHS LF@var%d\n",dest);
    default:
        break;
    }
    printf("POPFRAME\n");
}

void gen_local_scope(symtable_t *table) {
    printf("CREATEFRAME\n");
    table_traverse(table,&gen_var_copy);
    printf("PUSHFRAME\n");
}

void gen_drop_local_scope(symtable_t *table) {
    printf("POPFRAME\n");
    table_traverse(table,&gen_var_val_move);
}

void gen_cnd_jump(char *dest_type, int dest_number) {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS %s%d\n",dest_type, dest_number);
}

int get_cond_label() {
    static int cond_label_num = 0;
    cond_label_num ++;
    return cond_label_num;
}

void gen_cond_else_label(int cond_label_num) {
    printf("LABEL IF_ELSE%d\n",cond_label_num);
}

void gen_loop_label(int loop_label_num) {
    printf("LABEL Loop%d\n",loop_label_num);
}

int get_loop_label_num() {
    static int loop_label_num = 0;
    loop_label_num++;
    return loop_label_num;
}

void gen_cond_end_label(int cond_label_num) {
    printf("LABEL IF_END%d\n",cond_label_num);
}

void gen_jmp(int label_num) {
    printf("JUMP IF_END%d\n",label_num);
}

void gen_func_def(param_t **params,char *name) {
    printf("LABEL %s\n",name);
    for (int param_idx = 0; params[param_idx] != NULL; param_idx++) {
        printf("DEFVAR LF@%s\n",params[param_idx]->id);
        printf("MOVE LF@%s TF@%s\n",params[param_idx]->id,params[param_idx]->id);
    }
    printf("PUSHFRAME\n");

}

void gen_func_return() {
    printf("POPFRAME\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}


void gen_func_pre_call(param_t **params) {
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    for (int param_idx = 0; params[param_idx] != NULL; param_idx++) {
        printf("DEFVAR TF@%s\n",params[param_idx]->id);
    }
}

void add_func_arg(char *name) {
    printf("POPS TF@%s\n",name);
}

void gen_func_call(char *name) {
    printf("CALL %s\n",name);
}

int get_new_var() {
    static int var_number = 0;
    var_number++;
    return var_number;
}

void gen_read(char *identifier, bool global, const char *type) {
    const char *scope = global ? "GF" : "LF";
    printf("READ %s@%s %s\n",scope,identifier, type);
}

void gen_write_var(char *identifier, bool global) {
    const char *scope = global ? "GF" : "LF";
    printf("WRITE %s@%s \n",scope,identifier);
}

void gen_write_lit(litValue value, lit_type_t type) {
    switch (type) {
        case INT_LIT:
            printf("WRITE int@%d\n",value.i);
            break;

        case DOUBLE_LIT:
            printf("WRITE float@%a\n",value.d);
            break;
        case STRING_LIT:
            printf("WRITE string@%s\n",value.str);
            break;
    default:
        break;
    }
    printf("POPFRAME\n");
}