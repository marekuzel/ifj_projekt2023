#ifndef SYMTABLE_H
#define SYMTABLE_H
#define TABLE_SIZE 256
#include <stdbool.h>
#include "utils.h"

/**
 * 
*/
typedef enum { 
    INT, 
    STRING,
    DOUBLE,
    FUNC
    } entry_type;

/**
 * 
*/
typedef struct  {
    char *name;
    char *id;
    entry_type type;
} param_t;

/**
 * 
*/
typedef struct symtable_entry {
    entry_type type;
    bool defined;
    bool declared;
    param_t **params;
    tokenValue value;
} symtable_entry_t;

/**
 * 
*/
typedef struct awl_tree {
  char *key;               
  symtable_entry_t *value;              
  int height;
  struct awl_tree *left;  
  struct awl_tree *right; 
} awl_t;


typedef struct {
    awl_t *global_table;
    awl_t **table_stack;
    int size;
    int top_idx;
} symtable_t;


/**
 * 
*/
void awl_init(awl_t **awl);

/**
 * 
*/
void awl_insert(awl_t **awl, char *key, symtable_entry_t *entry);

/**
 * 
*/
bool awl_search(awl_t *awl, char *key, symtable_entry_t **entry);

/**
 * 
*/
void awl_delete(awl_t **awl, char key);

/**
 * 
*/
void awl_dispose(awl_t **awl);

/**
 * 
*/
int balance_node(awl_t* awl);

/**
 * 
*/
int key_cmp(const char *key, const char *cmp);

/**
 * 
*/
int get_height(awl_t *awl);

/**
 * 
*/
int max(int a, int b);

/**
 * 
*/
awl_t *awl_rotate_right(awl_t *target);

/**
 * 
*/
awl_t *awl_rotate_left(awl_t *target);

/**
 * 
*/
int balance_node(awl_t *awl);

/**
 * 
*/
void awl_balance(awl_t **awl, const char *key);

/**
 * 
*/
void table_insert(symtable_t *table, char *key, symtable_entry_t **entry);

/**
 * 
*/
void table_add_scope(symtable_t *table);

/**
 * 
*/
void table_remove_scope(symtable_t *table);

/**
 * 
*/
void table_insert_global(symtable_t *table, char *key, symtable_entry_t **entry);

/**
 * @param params: array NULL treminated array of pointers to param_t
*/
void table_function_insert(symtable_t *table, char *key, param_t **parms);

/**
 * 
*/
bool table_search(symtable_t *table, char *key, symtable_entry_t **entry);

/**
 * 
*/
bool table_search_global(symtable_t *table, char *key, symtable_entry_t **entry);

/**
 * 
*/
void table_init(symtable_t *table);

/**
 * 
*/
void table_dispose(symtable_t *table);

/**
 * 
*/
symtable_entry_t *entry_create(void);

/**
 * 
*/
void entry_dispose(symtable_entry_t *entry);
#endif