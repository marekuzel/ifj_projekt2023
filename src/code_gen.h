#include "utils.h"
#include "symtable.h"

#define LOOP_L "LOOP"
#define LOOP_END_L "LOOP_END"
#define ELSE_L "IF_ELSE"
#define IF_L "IF"
#define IF_END_L "IF_END"

/**
 * @brief enum for types of conversions
*/
typedef enum conv_type {
    FI, //float to int
    IF, //int ot float
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
    
/**
 * @brief function generating code for binary operation
 * 
 * @param operator: accepted operator values
 *      '+'  - adition
 *      '-'  - subtraction
 *      '*'  - multiplication
 *      '/'  - division
 *      '?'  - ??
 *      '|'  - string concatenation
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
void gen_push_lit(litValue value, TokenType type);


/**
 * @brief function generates code for built-in read functions
 * 
 * @param type: type of value to read only accepted values:
 *          "int" - integer
 *          "float" - float
 *          "sting" - string 
*/
void gen_read(char *type);

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
 * @param name: function name
 * 
*/
void gen_func_def(char *name);

/**
 * @brief function generates code for function return
*/
void gen_func_return();

/**
 * @brief function generates code for function calls
 * 
 * @param name: funcion name
 * @param entry: symtable entry for funcion
*/
void gen_func_call(char *name, symtable_entry_t *entry);

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
 * @brief function generates code for write function call
 * 
 * @param entry: a pointer to a symtable entry
*/

void gen_write_arg(param_t *param);

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

/**
 * @brief funcion generates code for built-in funcion int2double
*/
void gen_int2double();

/**
 * @brief funcion generates code for built-in funcion double2int
*/
void gen_double2int();

int get_cont_label();

void gen_cont_label(int cont_label_num);

void jump_cont_label(int cont_label_num);