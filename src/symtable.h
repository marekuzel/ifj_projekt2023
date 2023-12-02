#ifndef SYMTABLE_H
#define SYMTABLE_H
#define SYMTABLE_SIZE 256
#include <stdbool.h>
#include "utils.h"

/*
**************
Symtable entry 
**************
*/

/**
 * @brief struct representing a function parameter
*/
typedef struct  param{
    char *name; //parameter name
    char *id; //parameter identifier
    bool global; //flag if variable is global
    bool var; //flag if the param is a variable
    TokenType type; //parameter type
    litValue value; //value of literal params
} param_t;


/**
 * @brief struct representing a sytable entry
*/
typedef struct symtable_entry {
    TokenType type; //type of entry
    bool defined; //flag for symbol definition
    bool declared; //flag for symbol declaration
    bool redeclared; //flag for symbol redeclaration
    bool constant; //flag for denouncing constants
    bool modified; //flag denounces modificatoin in value
    param_t **params; // NULL termianted array of function parameters
    TokenType return_type; //return type of a function
} symtable_entry_t;


/**
 * @brief function for creating a table entry
 * 
 * @return pointer to a new table entry
*/
symtable_entry_t *entry_create(void);

/**
 * @brief function for safely disposing of a table entry
*/
void entry_dispose(symtable_entry_t *entry);


/*
*********
awl tree
*********
*/

/**
 * @brief struct representing an awl binary search tree
*/
typedef struct awl_tree {
  char *key; //node key 
  symtable_entry_t *value; //node data
  int height; //node height
  struct awl_tree *left; //left child
  struct awl_tree *right; // rigth child
} awl_t;

/**
 * @brief struct representing a symtable
*/
typedef struct symtable{
    awl_t **table_stack; //stack of symtables
    int size; //size of table_stack
    int top_idx; //current index to the table_stack
} symtable_t;

typedef void (*action_t) (awl_t *awl);
/*
***************************
awl tree -helper functions
***************************
*/

/**
 * @brief function used for key comparison
 * 
 * @par Function compares 'key' to 'cmp' character by charcter.
 *       if key == cmp returns 0
 *       if key > cmp returns 1
 *       if key < cmp return -1
 *
 * @param key: searched key
 * @param cmp: key to compare against 
 * 
 * @return 0 if keys equal, 1 if key > cmp, -1 if key < cmp 
*/
int key_cmp(const char *key, const char *cmp);

/**
 * @brief function for geting heiht of a node
 * 
 * @par Function retuns height of a node if node
 *      is not NULL, otherwise returns 0
 * 
 * @param awl: pointer to node of awl tree
 * 
 * @return 0 is node is NULL, node->height otherwise
*/
int get_height(awl_t *awl);

/**
 * @brief helper function, retuns max(a,b)
*/
int max(int a, int b);

/**
 * @brief helper function for determining balance of a node of awl tree
 * 
 * @param awl: pointer to node of awl tree;
*/
int balance_node(awl_t* awl);

/**
 * @brief function for initializng an awl tree
 * 
 * @param[out] awl: pointer to node of awl tree
*/
void awl_init(awl_t **awl);

/**
 * @brief function for searching awl tree
 * 
 * @par Function searches the awl tree for key. If key found returns true
 *      and data at key are in 'entry' If key is not found entry is undefined
 * 
 * @param[in] awl: pointer to node of awl tree
 * @param[in] key: key to find
 * @param[out] entry: pointer to the data if found
 * 
 * @return true if key found, false othwerwise
*/
bool awl_search(awl_t *awl, char *key, symtable_entry_t **entry);

/**
 * @brief function performs right rotation on awl tree
 * 
 * @param target: pointer to the target of the rotation
 * 
 * @return rotated subtree of target
*/
awl_t *awl_rotate_right(awl_t *target);

/**
 * @brief function performs left rotation on awl tree
 * 
 * @param target: pointer to the target of the rotation
 * 
 * @return rotated subtree of target
*/
awl_t *awl_rotate_left(awl_t *target);

/**
 * @brief function for balancing the awl tree
 * 
 * @param[out] awl: pointer to a node of awl tree
 * @param[in] key: key of a node to balance
*/
void awl_balance(awl_t **awl, const char *key);

/**
 * @brief function for inseting/updating a data in awl_tree
 * 
 * @param[out] awl: pointer to node of awl tree
 * @param[in] key: key to insert
 * @param[in] entry: data to insert
*/
void awl_insert(awl_t **awl, char *key, symtable_entry_t *entry);

/**
 * @brief function for safely disposing the awl tree
 * 
 * @param[out] awl: pointer to node of awl tree
*/
void awl_dispose(awl_t **awl);



/**
 * @brief function for initializing a symtable
 * 
 * @param table: pointer to a table
*/
void table_init(symtable_t *table);

/**
 * @brief fucntion for adding a new scope into the table
 * 
 * @param talbe: pointer to a table
*/
void table_add_scope(symtable_t *table);

/**
 * @brief function for removing a scope from the table
 * 
 * @param table: pointer to a table
*/
void table_remove_scope(symtable_t *table);

/**
 * @brief function for inserting a key value pair to symtable
 * 
 * @par Function insests a key value pair into the local symtable and 
 *      pointer to data is in entry.
 *      Insertion happens in the top most local table, if trying to update
 *      an entry use table_search
 *       
 * @param[in] table: pointer to a symtable
 * @param[in] key: symbol to insert
 * @param[out] entry: pointer to data at key 
*/
void table_insert(symtable_t *table, char *key, symtable_entry_t **entry);

/**
 * @brief function for inseting a key value pair to the global symtable
 * 
 * @par Function insests a key value pair into the global symtable and 
 *      pointer to data is in entry.
 * 
 * @param[in] table: pointer to a symtable
 * @param[in] key: symbol to insert
 * @param[out] entry: pointer to data at key 
*/
void table_insert_global(symtable_t *table, char *key, symtable_entry_t **entry);

/**
 * @brief function for inserting a function into a symtable
 * 
 * 
 * @param table: pointer to a table
 * @param key: function name
 * @param params: array NULL treminated array of function parameters
*/
void table_function_insert(symtable_t *table, char *key, param_t **parms, TokenType return_type);

/**
 * @brief function for searching a symbol in table
 * 
 * @par Function searches the entire table for a given symbol, if the symbol is found
 *      returns true and data a symbol are in entry. if symbol is not found returns false
 *      and enttry is undefined
 * 
 * @param[in] table: pointer to table
 * @param[in] key: symbol to find
 * @param[out] entry: pointer to data at key
 * 
 * @return true if sybol is found flase otherwise
*/
bool table_search(symtable_t *table, char *key, symtable_entry_t **entry);

/**
 * @brief function for searching a symbol in table
 * 
 * @par Function searches just the table for a given symbol, if the symbol is found
 *      returns true and data a symbol are in entry. if symbol is not found returns false
 *      and enttry is undefined
 * 
 * @param[in] table: pointer to table
 * @param[in] key: symbol to find
 * @param[out] entry: pointer to data at key
 * 
 * * @return true if sybol is found flase otherwise
*/
bool table_search_global(symtable_t *table, char *key, symtable_entry_t **entry);

/**
 * @brief function for sefely disposing of the table
 * 
 * @param table: pointer to a table
*/
void table_dispose(symtable_t *table);

/**
 * @brief function for traversing the table and preforming an action
 * 
 * @param table: a pointer to a symtable
 * @param action: action to be performend (function pointer)
*/
void table_traverse(symtable_t *table, action_t action);

/**
 * @brief ADT for dynamiclaly growing array of function parameters
*/
typedef struct param_buffer_t {
    param_t **bytes; //array of params
    int cap; //max capacity
    int length; //total length
} ParamBufferT;

/**
 * @brief Initilaize ADT paramBuffer
 * 
 * @param buffer pointer to buffer
 * @retval ret_t BUFF_INIT_SUCCES if succesfull
 * @retval ret_t BUFF_INIT_FAIL if failed
 */
buff_ret_t param_buffer_init(ParamBufferT *buffer);

/**
 * @brief Appends a param_t to paramBuffer 
 * 
 * @param buffer pointer to buffer
 * @param param malloced param
 * @retval ret_t BUFF_APPEND_SUCCES if succesfull
 * @retval ret_t BUFF_APPEND_FAIL if failed
 */

buff_ret_t table_insert_param(ParamBufferT *buffer, param_t *param);

/**
 * @brief disposes of ParamBuffer
 * 
 * @param buffer: pointer to a ParamBuffer
*/
void param_buffer_detor(ParamBufferT *buffer);

/**
 * @brief funcion for creating a param from string literals
 * 
 * @param id: parameter identificator (never NULL)
 * @param name: parameter name (optionaly  NULL)
 * @param type: type of param
 * 
*/
param_t *param_from_lit_create(char *id, char *name, TokenType type);


/**
 * @brief funcion for creating a param
 * 
 * @param id: parameter identificator (never NULL)
 * @param name: parameter name (optionaly  NULL)
 * @param type: type of param
 * 
*/
param_t *param_create(char *id, char *name, TokenType type);

/**
 * @brief function for adding vlaues to parameters
 * 
 * @param table: a pointer to a symtable
 * @param param: pointer to a fucntion parameter
 * @param value: enum with litralValues 
 * @param type: type of literal value
*/
void param_value_init(symtable_t *table, param_t *param, litValue value, TokenType type);

/**
 * @brief function inserts builtin funcion entries into the table
 * 
 * @param table: pointer to a symtable
 * 
 * @returns 1 if an error occured otherwise 0
*/
int table_insert_builtin_funcs(symtable_t *table);

/**
 * @brief function checks wether a given "name" is global or not
 * 
 * @param table: a pointer to a symtable
 * @param name: name to find
 * 
 * @returns true if given name is global false otherwise 
*/
bool is_global(symtable_t *table, char *name);

/**
 * @brief function for transforming a string literal into a heap allocated string
*/
char *lit2ptr(const char *lit);

#define CHECK_ERR(err) \
    if (ret != 0)      \
        goto err;       \


param_t **param_buffer_export(ParamBufferT *);

//TODO vymaz
int insert_builtin(symtable_t *table, char *name, TokenType ret_type, param_t **params, int param_num);

void add_params_to_scope(symtable_t *table, symtable_entry_t *entry);
#endif