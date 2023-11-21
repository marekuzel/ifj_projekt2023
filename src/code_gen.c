#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "code_gen.h"


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
    case INT_T:
        printf("PUSHS int@%d\n",value.i);
        break;
    
    case DOUBLE_T:
        printf("PUSHS float@%lf\n",value.d);
        break;

    case STRING_T:
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


/*broken to be fixed!!!!*/
// void gen_statements(ast_node_t *ast) {
//     ast_node_t *tmp = ast;
//     while (tmp->next != NULL)
//     {
//         gen_statement(tmp);
//         tmp = tmp->next;
//     }
    
// }

// void gen_loop(ast_node_t *ast) {

//     static int loop_label_num = 0;
//     loop_label_num++;

//     printf("LABEL Loop%d\n",loop_label_num);

//     gen_statements(ast->right);
    
//     gen_cond(ast->left,"Loop",loop_label_num);
// }


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

// int get_new_var() {
//     static int var_number = 0;
//     var_number++;
//     return var_number;
// }

// void gen_read(ast_node_t *ast, const char *type) {
//     printf("READ LF@%s %s\n",ast->identifier, type);
// }



// void gen_write(ast_node_t *ast) {
//     printf("WRITE %s\n",ast->value.str);
// }

// void gen_string_op(ast_node_t *ast) {

//     int var_number1 = get_new_var();
//     int var_number2 = get_new_var();
//     int var_number3 = get_new_var();

//     switch (ast->operator) {
//     case 'l':
//         printf("DEFVAR LF@var%d\n",var_number1);
//         printf("DEFVAR LF@var%d\n",var_number2);

//         printf("POPS LF@var%d\n",var_number2);  
//         printf("POPS LF@var%d\n",var_number1);

//         printf("STRLEN LF@var%d LF@var%d\n",var_number1,var_number2);
//         printf("PUSHS LF@var%d\n",var_number1);
//         break;
//     case '|':
//         printf("DEFVAR LF@var%d\n",var_number1);
//         printf("DEFVAR LF@var%d\n",var_number2);
//         printf("DEFVAR LF@var%d\n",var_number3);

//         printf("POPS LF@var%d\n",var_number3);  
//         printf("POPS LF@var%d\n",var_number2);  
//         printf("POPS LF@var%d\n",var_number1);

//         printf("CONCAT LF@var%d LF@var%d LF@var%d\n",var_number1, var_number2, var_number3);
//         printf("PUSHS LF@var%d\n",var_number3);
//         break;
//     case 'g':
//         printf("DEFVAR LF@var%d\n",var_number1);
//         printf("DEFVAR LF@var%d\n",var_number2);
//         printf("DEFVAR LF@var%d\n",var_number3);

//         printf("POPS LF@var%d\n",var_number3);  
//         printf("POPS LF@var%d\n",var_number2);  
//         printf("POPS LF@var%d\n",var_number1);

//         printf("GETCHAR LF@var%d LF@var%d LF@var%d\n",var_number1, var_number2, var_number3);
//         printf("PUSHS LF@var%d\n",var_number3);
//         break;
    
//     case 's':
//         printf("DEFVAR LF@var%d\n",var_number1);
//         printf("DEFVAR LF@var%d\n",var_number2);
//         printf("DEFVAR LF@var%d\n",var_number3);

//         printf("POPS LF@var%d\n",var_number3);  
//         printf("POPS LF@var%d\n",var_number2);  
//         printf("POPS LF@var%d\n",var_number1);

//         printf("SETCHAR LF@var%d LF@var%d LF@var%d\n",var_number1, var_number2, var_number3);
//         printf("PUSHS LF@var%d\n",var_number3);
//     default:
//         break;
//     }
// }



// void gen_function_call(ast_node_t *ast) {

//     if (strcmp("readString",ast->identifier) == 0) {
//         gen_read(ast,"string");
//     } else if (strcmp("readInt",ast->identifier) == 0) {
//         gen_read(ast,"int");
//     } else if (strcmp("readDouble",ast->identifier) == 0) {
//         gen_read(ast,"double");
//     } else if (strcmp("write",ast->identifier) == 0) {
//         gen_write(ast);
//     }
    
    
// }