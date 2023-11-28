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
    gen_ord();
    gen_chr();
    gen_substring();
}

void gen_assignment(char *identifier, bool global) {
    char *scope = global ? "GF" : "LF";
    printf("POPS %s@%s\n",scope,identifier);
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

void gen_push_var(char *id, bool global){
    const char *scope = global ? "GF" : "LF";
    printf("PUSHS %s@%s\n",scope,id);
}


LIT_OP(push,"PUSHS")

void gen_var_copy(awl_t *awl) {
    if (awl->value->redclared || awl->value->type == TOKEN_FUNC)
        return;

    printf("DEFVAR TF@%s\n",awl->key);
    printf("MOVE TF@%s LF@%s\n",awl->key,awl->key);
}

void gen_var_val_move(awl_t *awl) {
    if (awl->value->redclared || awl->value->type == TOKEN_FUNC)
        return;

    printf("MOVE LF@%s TF@%s\n",awl->key,awl->key);
}

void gen_expr_binop(char operator) {

    int label_num;
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

    case '\\': //ak delia floaty 
        printf("DIVS\n");
        break;

    case '?':
        printf("CREATEFRAME\n");
        printf("PSUHFRAME\n");
        label_num = get_cond_label();

        printf("DEFVAR LF@va\n");
        printf("DEFVAR LF@vb\n");
        printf("DEFVAR LF@result\n");

        printf("POPS LF@b\n");
        printf("POPS LF@a\n");
        printf("MOVE LF@result LF@a\n");

        printf("JUMPIFEQ dubquestion%d LF@a nil@nil",label_num);
        printf("MOVE LF@result LF@b\n");

        printf("PUSHS LF@result\n");
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

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@dest\n");
    printf("DEFVAR LF@op1\n");

    printf("POPS LF@op1\n");

    switch (operator) {
    case 'l':
        printf("POPS LF@dest\n");
        printf("STRLEN LF@dest LF@op1\n");
        break;

    case '|':
        printf("DEFVAR LF@op2\n");

        printf("POPS LF@op2\n"); 
        printf("POPS LF@dest\n");

        printf("CONCAT LF@dest LF@op1 LF@op2\n");
        break;
    case 'g':
        printf("DEFVAR LF@op2\n");

        printf("POPS LF@op2\n"); 
        printf("POPS LF@dest\n");

        printf("GETCHAR LF@dest LF@op1 LF@op2\n");
        break;
    
    case 's':
        printf("DEFVAR LF@op2\n");

        printf("POPS LF@op2\n"); 
        printf("POPS LF@dest\n");

        printf("SETCHAR LF@dest LF@op1 LF@op2\n");
    default:
        break;
    }

    printf("PUSHS LF@dest\n");
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
    printf("LABEL LOOP%d\n",loop_label_num);
}

int get_loop_label() {
    static int loop_label_num = 0;
    loop_label_num++;
    return loop_label_num;
}

void gen_end_label(char *des_type, int label_num) {
    printf("LABEL %s_END%d\n",des_type,label_num);
}

void gen_jmp(char* label_type, int label_num) {
    printf("JUMP %s%d\n",label_type,label_num);
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

void add_arg(param_t *param) {
    const char *scope;
    switch (param->type) {
    case TOKEN_INTEGER:
        printf("MOVE TF@%s int@%d\n", param->id, param->value.i);
        break;
    case TOKEN_DOUBLE:
        printf("MOVE TF@%s float@%a\n", param->id, param->value.d);
        break;
    case TOKEN_STRING:
        printf("MOVE TF@%s string@%s\n", param->id, param->value.str);
        break;
    case TOKEN_NIL:
        printf("MOVE TF@%s nil@nil\n",param->id);
        break;
    default:
        scope = param->global ? "GF" : "LF";
        printf("MOVE TF@%s %s@%s\n",param->id,scope,param->value.str);
        break;
    }
}

void gen_func_call(char *name, symtable_entry_t *entry) {

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    param_t *param;
    for (int param_idx = 0; entry->params[param_idx] != NULL; param_idx++) {
        param = entry->params[param_idx];
        printf("DEFVAR TF@%s\n",param->id);
        add_arg(param);
    }
    printf("CALL %s\n",name);
}


void gen_read(char *identifier, bool global, char *type) {
    const char *scope = global ? "GF" : "LF";
    printf("READ %s@%s %s\n",scope,identifier, type);
}

void gen_write_var(char *identifier, bool global) {
    const char *scope = global ? "GF" : "LF";
    printf("WRITE %s@%s \n",scope,identifier);
}

LIT_OP(write,"WRITE")


void gen_substring() {
    printf("LABEL substring\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@boolflag\n");
    printf("DEFVAR LF@strlen\n");
    printf("DEFVAR LF@result\n");
    printf("MOVE LF@result nil@nil\n");
    printf("DEFVAR LF@tmp\n");

    printf("LT LF@boolflag LF@i int@0\n");
    printf("JUMPIFEQ substringend LF@boolflag bool@true\n");

    printf("LT LF@boolfalg LF@j int@0\n");
    printf("JUMPIFEQ substringend LF@boolflag bool@true\n");

    printf("STRLEN LF@strlen LF@s\n");
    printf("LT LF@boolflag LF@i LF@strlen\n");
    printf("JUMPIFNEQ substringend LF@boolflag bool@true\n");

    printf("LT LF@boolflag LF@j LF@strlen\n");
    printf("JUMPIFNEQ substringend LF@boolflag bool@true\n");

    printf("MOVE LF@result string@\n");

    printf("LABEL substringloop\n");
    printf("LT LF@boolflag LF@i LF@j\n");
    printf("JUMPIFNEQ substringloopend LF@boolflag bool@true\n");
    printf("GETCHAR LF@tmp LF@s LF@i\n");
    printf("CONCAT LF@result LF@result LF@tmp\n");
    printf("ADD LF@i LF@i int@1\n");
    printf("JUMP substringloop\n");

    printf("PUSHS LF@result\n");
    printf("LABEL substingend\n");
    printf("POPFRAME\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void gen_ord() {
    printf("LABEL ord\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@strlen\n");
    printf("DEFVAR LF@boolfag\n");
    printf("DEFVAR LF@result\n");


    printf("MOVE LF@result int@0\n");
    printf("GT LF@boolflag LF@strlen int@0\n");
    printf("JUMPIFEQ ordend LF@boolflag bool@false\n");

    printf("STRI2INT LF@result LF@c int@0\n");

    printf("LABEL ordend\n");
    printf("PUSHS LF@result\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}


void gen_chr() {
    printf("LABEL chr\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@boolflag\n");
    printf("DEFVAR LF@result\n");

    printf("MOVE LF@result LF@i\n");
    printf("GT LF@boolflag LF@i int@255\n");
    printf("JUMPIFEQ chrend LF@boolflag bool@true\n");
    printf("LT LF@boolflag LF@i int@0\n");
    printf("JUMPIFEQ chrend LF@boolflag bool@true\n");

    printf("INT2CHAR LF@reuslt LF@i\n");

    printf("LABEL chrend\n");
    printf("PUSHS LF@result\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}