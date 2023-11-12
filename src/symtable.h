#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>


typedef enum {INT, STRING, DOUBLE, FUNC} entry_type;


typedef struct  {
    char* name;
    entry_type type;
} param_t;

typedef struct symtable_entry {
    entry_type type;
    bool dfined;
    bool declared;
    param_t *params;
} symtable_entry_t;


typedef struct symtable {
  char *key;               
  int value;              
  int height;
  struct symtable *left;  
  struct symtable *right; 
} symtable_t;


void table_init(symtable_t **table);
void table_insert(symtable_t **table, char *key, int value);
bool table_search(symtable_t *table, char *key, int *value);
void table_delete(symtable_t **table, char key);
void table_dispose(symtable_t **table);
int balance_node(symtable_t* table);
int key_cmp(const char *key, const char *cmp);
void table_replace_by_rightmost(symtable_t *target, symtable_t **table);
#endif