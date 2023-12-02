#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "code_gen.h"
#include "symtable.h"


void gen_prog() {
    printf(".IFJcode23\n");
    printf("DEFVAR GF@$boolflag\n");
    printf("DEFVAR GF@$dest\n");
    printf("DEFVAR GF@$op1\n");
    printf("DEFVAR GF@$op2\n");
    jump_cont_label(1);

}
void gen_prog_end(int exit_code) {
    int cnt_label = get_cont_label();
    gen_cont_label(cnt_label);
    printf("EXIT int@%d\n",exit_code);
    gen_ord();
    gen_chr();
    gen_substring();
    gen_int2double();
    gen_double2int();
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


void  gen_push_lit(litValue value, TokenType type) {
        switch (type) {
            case TOKEN_INTEGER:
                printf("PUSHS int@%d\n",value.i);
                break;

            case TOKEN_DOUBLE:
                printf("PUSHS float@%a\n",value.d);
                break;

            case TOKEN_STRING:
                printf("PUSHS string@%s\n",value.str);
                break;

            default:
                printf("PUSHS nil@nil\n");
                break;
        }
    }

void gen_var_copy(awl_t *awl) {
    if (awl->value->redeclared|| awl->value->type == TOKEN_FUNC) {
        return;
    }
    printf("DEFVAR TF@%s\n",awl->key);
    printf("MOVE TF@%s LF@%s\n",awl->key,awl->key);
}

void gen_var_val_move(awl_t *awl) {
    if (awl->value->redeclared || awl->value->type == TOKEN_FUNC || !(awl->value->modified)) {
        return;
    }
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
        printf("DIVS\n");
        break;
    
    case '\\':
        printf("IDIVS\n");
        break;

    case '?':
        label_num = get_cond_label();
        printf("POPS GF@$op2\n");
        printf("POPS GF@$op1\n");
        
        printf("EQ GF@$boolflag GF@$op1 nil@nil\n");
        printf("JUMPIFNEQ dubquestion%d GF@$boolflag bool@true\n",label_num);
        printf("MOVE GF@$op1 GF@$op2\n");
        printf("LABEL dubquestion%d\n",label_num);
        printf("PUSHS GF@$op1\n");
        break;

    case '|':
        printf("MOVE GF@$dest string@\n");

        printf("POPS GF@$op2\n"); 
        printf("POPS GF@$op1\n");

        printf("CONCAT GF@$dest GF@$op1 GF@$op2\n");

        printf("PUSHS GF@$dest\n");
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

void gen_local_scope(symtable_t *table) {
    printf("CREATEFRAME\n");
    table_traverse(table,&gen_var_copy);
    printf("PUSHFRAME\n");
}

void gen_drop_local_scope(symtable_t *table) {
    printf("POPFRAME\n");
    table_traverse(table,&gen_var_val_move);
    printf("CREATEFRAME\n");
}

void gen_cnd_jump(char *dest_type, int dest_number) {
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $%s%d\n",dest_type, dest_number);
}

int get_cond_label() {
    static int cond_label_num = 0;
    cond_label_num ++;
    return cond_label_num;
}

void gen_cond_else_label(int cond_label_num) {
    printf("LABEL $IF_ELSE%d\n",cond_label_num);
}

void gen_loop_label(int loop_label_num) {
    printf("LABEL $LOOP%d\n",loop_label_num);
}

int get_loop_label() {
    static int loop_label_num = 0;
    loop_label_num++;
    return loop_label_num;
}

void gen_end_label(char *des_type, int label_num) {
    printf("LABEL $%s_END%d\n",des_type,label_num);
}

void gen_jmp(char* label_type, int label_num) {
    printf("JUMP $%s%d\n",label_type,label_num);
}

void gen_func_def(char *name) {
    printf("LABEL %s\n",name);
    printf("PUSHFRAME\n");

}

void gen_func_return() {
    printf("POPFRAME\n");
    printf("CREATEFRAME\n");
    printf("RETURN\n");
}

void add_arg(param_t *param) {
    if (param->var) {
        char *scope = param->global ? "GF" : "LF";
        printf("MOVE TF@%s %s@%s\n",param->id,scope,param->value.str);
        return;
    }

    switch (param->type) {
    case TOKEN_DT_INT:
        printf("MOVE TF@%s int@%d\n", param->id, param->value.i);
        break;
    case TOKEN_DT_DOUBLE:
        printf("MOVE TF@%s float@%a\n", param->id, param->value.d);
        break;
    case TOKEN_DT_STRING:
        printf("MOVE TF@%s string@%s\n", param->id, param->value.str);
        break;
    case TOKEN_NIL:
        printf("MOVE TF@%s nil@nil\n",param->id);
        break;
    default:
        break;
    }
}

void gen_general_func_call(char *name, symtable_entry_t *entry) {
    printf("CREATEFRAME\n");
    param_t *param;
    for (int param_idx = 0; entry->params[param_idx] != NULL; param_idx++) {
        param = entry->params[param_idx];
        printf("DEFVAR TF@%s\n",param->id);
        add_arg(param);
    }
    printf("CALL %s\n",name);
}

void gen_func_call(char *name, symtable_entry_t *entry) {
    if (strcmp(name,"readString") == 0) {
        gen_read("string");
    } else if (strcmp(name, "readInt") == 0) {
        gen_read("int");
    } else if (strcmp(name, "readDouble") == 0) {
        gen_read("float");
    } else {
        gen_general_func_call(name,entry);
    }
}

void gen_read(char *type) {
    printf("READ GF@$dest %s\n",type);
    printf("PUSHS GF$dest\n");
}

void gen_write_arg(param_t *param) {
    const char *scope;
    switch (param->type) {
    case TOKEN_INTEGER:
        printf("WRITE int@%d\n",param->value.i);
        break;
    case TOKEN_DOUBLE:
        printf("WRITE float@%a\n",param->value.d);
        break;
    case TOKEN_STRING:
        printf("WRITE string@%s\n",param->value.str);
        break;
    case TOKEN_NIL:
        printf("WRITE nil@nil\n");
        break;
    default:
        scope = param->global ? "GF" : "LF";
        printf("WRITE %s@%s\n",scope,param->value.str);
        break;
    }
}

void gen_substring() {
    printf("LABEL substring\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@strlen\n");
    printf("MOVE GF@$dest nil@nil\n");
    printf("DEFVAR LF@tmp\n");

    printf("LT GF@$boolflag LF@i int@0\n");
    printf("JUMPIFEQ substringend GF@$boolflag bool@true\n");

    printf("LT GF@$boolflag LF@j int@0\n");
    printf("JUMPIFEQ substringend GF@$boolflag bool@true\n");

    printf("STRLEN LF@strlen LF@s\n");
    printf("LT GF@$boolflag LF@i LF@strlen\n");
    printf("JUMPIFNEQ substringend GF@$boolflag bool@true\n");

    printf("GT GF@$boolflag LF@j LF@strlen\n");
    printf("JUMPIFEQ substringend GF@$boolflag bool@true\n");

    printf("MOVE GF@$dest string@\n");

    printf("LABEL substringloop\n");
    printf("LT GF@$boolflag LF@i LF@j\n");
    printf("JUMPIFNEQ substringend GF@$boolflag bool@true\n");
    printf("GETCHAR LF@tmp LF@s LF@i\n");
    printf("CONCAT GF@$dest GF@$dest LF@tmp\n");
    printf("ADD LF@i LF@i int@1\n");
    printf("JUMP substringloop\n");

    printf("LABEL substringend\n");
    printf("PUSHS GF@$dest\n");
    gen_func_return();
}

void gen_ord() {
    printf("LABEL ord\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR LF@strlen\n");
    printf("STRLEN LF@strlen LF@c\n");
    printf("MOVE GF@$dest int@0\n");
    printf("GT GF@$boolflag LF@strlen int@0\n");
    printf("JUMPIFEQ ordend GF@$boolflag bool@false\n");

    printf("STRI2INT GF@$dest LF@c int@0\n");

    printf("LABEL ordend\n");
    printf("PUSHS GF@$dest\n");
    gen_func_return();
}


void gen_chr() {
    gen_func_def("chr");

    printf("MOVE GF@$dest LF@i\n");
    printf("GT GF@$boolflag LF@i int@255\n");
    printf("JUMPIFEQ chrend GF@$boolflag bool@true\n");
    printf("LT GF@$boolflag LF@i int@0\n");
    printf("JUMPIFEQ chrend GF@$boolflag bool@true\n");

    printf("INT2CHAR GF@$dest LF@i\n");

    printf("LABEL chrend\n");
    printf("PUSHS GF@$dest\n");

    gen_func_return();
}

void gen_int2double() {
    gen_func_def("Int2Double");

    printf("INT2FLOAT GF@$dest LF@term\n");
    printf("PUSHS GF@$dest\n");

    gen_func_return();
}

void gen_double2int() {
    gen_func_def("Double2Int");

    printf("FLOAT2INT GF@$dest LF@term\n");
    printf("PUSHS GF@$dest\n");
    
    gen_func_return();
}

int get_cont_label() {
    static int cont_label_num = 0;
    cont_label_num++;
    return cont_label_num;
}

void gen_cont_label(int cont_label_num) {
    printf("LABEL $cont%d\n",cont_label_num);
}

void jump_cont_label(int cont_label_num) {
    printf("JUMP $cont%d\n",cont_label_num);
}