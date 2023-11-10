#ifndef IAL_BTREE_TEST_UTIL_H
#define IAL_BTREE_TEST_UTIL_H

#include "symtable.h"
#include <stdio.h>

#define TEST(NAME, DESCRIPTION)                                                \
  void NAME() {                                                                \
    printf("[%s] %s\n", #NAME, DESCRIPTION);                                   \
    symtable_t *test_tree;                                                 \

#define ENDTEST                                                                \
  printf("\n");                                                                \
  table_dispose(&test_tree);                                                     \
  }

typedef enum direction { left, right, none } direction_t;

void bst_print_subtree(symtable_t *tree, char *prefix, direction_t from);
void bst_print_tree(symtable_t *tree);
void bst_insert_many(symtable_t **tree, const char *keys[], const int values[],
                     int count);

bool tree_invariant(symtable_t *tree);
#endif
