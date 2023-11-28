#include "utils.h"
#include "symtable.h"

/**
 * @brief enum for types of conversions
*/
typedef enum conv_type {
    FI, //float to int
    IF, //int ot float
    IC, //int to character
    CI, //character to int
} conv_type_t;

/**
 * @brief enum for relation operatros
*/
typedef enum rel_op {
    LT, //<
    LTE, //<=
    GT, //>
    GTE,// >=
    EQ, //==
    NEQ //!=
} rel_op_t;

/**
 * @brief enum for types of literals
*/
typedef enum lit_type { 
    INT_LIT, //integer literal
    STRING_LIT, //srting literal
    DOUBLE_LIT, //double literal
    NILL //nil literal
} lit_type_t;

/*
Macro for generating repetatinve code
*/
#define LIT_OP(func_name, op)                                 \
    void  gen_##func_name##_lit(litValue value, lit_type_t type) { \
        switch (type) {                                       \
            case INT_LIT:                                     \
                printf("%s int@%d\n",op,value.i);             \
                break;                                        \
                                                              \
            case DOUBLE_LIT:                                  \
                printf("%s float@%a\n",op,value.d);           \
                break;                                        \
                                                              \
            case STRING_LIT:                                  \
                printf("%s string@%s\n",op,value.str);        \
                break;                                        \
                                                              \
            default:                                          \
                printf("%s nil@nil\n",op);                    \
                break;                                        \
        }                                                     \
    }
    
/**
 * @brief function generating code for binary operation
 * 
 * @param operator: accepted operator values
 *      '+'  - adition
 *      '-'  - subtraction
 *      '*'  - multiplication
 *      '/'  - division between floats
 *      '\\' - division between ints
 *      '?'  - ??
*/
void gen_expr_binop(const char operator);

/**
 * @brief funciton generating code for conversion functions
 * 
 * @param conversion_type @see conv_type_t
 * 
*/
void gen_expr_conv(conv_type_t conversion_type);

/**
 * @brief function generates code for conditions
 * 
 * @param relation_operator: @see rel_op_t
*/
void gen_cond(rel_op_t relation_operator);

/**
 * @brief funcion generates code for pushing a variable to stack
 * 
 * @param id: id of variable
 * @param global: boolean flag
*/
void gen_push_var(char *id, bool global);

/**
 * @brief funcion generates code for pushing a literal to stack
 * 
 * @param value: value of literal @see litValue
 * @param type: type of lieral @see lit_type_t
*/
void gen_push_lit(litValue value, lit_type_t type);

/**
 * @brief function generates code for string operations
 * 
 * @param operator: accepted operato values:
 *        'l' - srting length
 *        '|' - string concatenation
 *        'g' - get character at idx of string
 *        's' - set character at idx of string
*/
void gen_string_op(const char operator);

/**
 * @brief function generates code for built-in read functions
 * 
 * @param identifier: target for read value
 * @param global: boolean flag
 * @param type: type of value to read only accepted values:
 *          "int" - integer
 *          "float" - float
 *          "sting" - string 
*/
void gen_read(char *identifier, bool global, const char *type);

/**
 * @brief function generates code assigning a value to a variable
 * 
 * @param identifier: variable id
 * @param global: boolean flag
*/
void gen_assignment(char *identifier, bool global);

/**
 * @brief function generates code for varaible definition
 * 
 * @param id: variable id
 * @param global :boolean flag
 * @param type: variable type (for default initialization)
*/
void gen_def_var(char *id, bool global, TokenType type);

/**
 * @brief function generating unqie numbers for loop labels
 * 
 * @return unique loop label number
*/
int get_loop_label();

/**
 * @brief function generating unqie numbers for condition labels
 * 
 * @return unique condition label number
*/
int get_cond_label();

/**
 * @brief function generating unique loop labels
 * 
 * @param loop_label_num: unique loop label number
*/
void gen_loop_label(int loop_label_num);

/**
 * @brief function generates code for conditional jumps
 * 
 * @param dest_type: label where to jump
 * @param des_number: label number of destination
*/
void gen_cnd_jump(char *dest_type, int dest_number);

/**
 * @brief function generates code for dunction definition
 * 
 * @param params: NULL terminated array of function parameters
 * @param name: function name
 * 
*/
void gen_func_def(param_t **params, char *name);

/**
 * @brief function generates code for function return
*/
void gen_func_return();

/**
 * @brief fucntion generates code for prepairing function parameters
 * 
 * @param params:  NULL terminated array of function parameters
*/
void gen_func_pre_call(param_t **params);

/**
 * @brief function adds variable argument
 * 
 * @param arg_id: argument identifier
 * @param var_id: variable identifier
 * @param global: boolean flag
*/
void add_var_arg(char *arg_id, char *var_id, bool global);
/**
 * @brief function adds literal argument
 * 
 * @param id: arguemnt identifier
 * @param value: literal value @see litValue
 * @param type: type of literal @see lit_value_t
*/
void add_lit_arg(char *id, litValue value, lit_type_t type);

/**
 * @brief function generates code for function calls
 * 
 * @param name: funcion name
*/
void gen_func_call(char *name);

/**
 * @brief function used for copying variables between scopes
 * 
 * @param awl: pointer to a single scope of symtable @see symtable_t @see awl_t
*/
void gen_var_copy(awl_t *awl);


/**
 * @brief funcion generates program prolog
*/
void gen_prog();

/**
 * @brief function generates code for moving out of a local scope
 * 
 * @param table: pointer to a symtable @see symtable_t
*/
void gen_drop_local_scope(symtable_t *table);

/**
 * @brief function generates code for moving into a new local scope
 * 
 * @param table: pointer to a symtable @see symtable_t
*/
void gen_local_scope(symtable_t *table);

/**
 * @brief function generates code for writing a literal to stdout
 * 
 * @param vlaue: value of a literal @see litVlaue
*/
void gen_write_lit(litValue value, lit_type_t type);

/**
 * @brief function generates code for writing the contets od a varible to stdout
 * 
 * @param identifies: varible identifier
 * @param global: boolean flag
*/
void gen_write_var(char *identifier, bool global);

/**
 * @brief function generates unique if_else label
 * 
 * @param label_num: unique label number 
*/
void gen_cond_else_label(int label_num);

/**
 * @brief function generates unique end label
 * 
 * @param dest_type: type of label to generate
 * @param label_num: label number 
*/
void gen_end_label(char *des_type, int cond_label_num);

/**
 * @brief function generates unique loop label
 * 
 * @param label_num: unique label number 
*/
void gen_loop_label(int loop_label_num);

/**
 * @brief function generates jump to a label
 * 
 * @param label_type: type of label to jump to
 * @param label_num: label number 
*/
void gen_jmp(char* label_type, int label_num);

/**
 * @brief fucnction generates program exit + its epilog (definitons of built-in fucntions)
*/
void gen_prog_end(int exit_code);

/**
 * @brief fucntion generates code for builtin-in funciton substring
*/
void gen_substring();

/**
 *@brief function generates code for built-in function ord 
*/
void gen_ord();

/**
 * @brief fucntion generates code for built-in funcion chr
*/
void gen_chr();