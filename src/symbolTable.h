#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

typedef struct symbol_table {
  char key;               
  int value;              
  int height;
  struct symbol_table *left;  
  struct symbol_table *right; 
} symbol_table_t;

void table_init(symbol_table_t **table);
void table_insert(symbol_table_t **table, char key, int value);
bool table_search(symbol_table_t *table, char key, int *value);
void table_delete(symbol_table_t **table, char key);
void table_dispose(symbol_table_t **table);

void table_replace_by_rightmost(symbol_table_t *target, symbol_table_t **table);
#endif