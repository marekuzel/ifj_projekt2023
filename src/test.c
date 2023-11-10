#include "symtable.h"
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
const int base_data_count = 15;
const char *base_keys[] = {"Aa", "Bb", "Cc", "Dd", "Ee", "Ff", "Gg", "Hh",
                          "Ii", "Jj", "Kk", "Ll", "Mm", "Nn", "Oo"};
const int base_values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};


void init_test() {
  printf("Binary Search Tree - testing script\n");
  printf("-----------------------------------\n");
  printf("\n");
}

TEST(test_tree_init, "Initialize the tree")
table_init(&test_tree);
ENDTEST

TEST(test_tree_dispose_empty, "Dispose the tree")
table_init(&test_tree);
table_dispose(&test_tree);
ENDTEST

TEST(test_tree_search_empty, "Search in an empty tree (A)")
table_init(&test_tree);
int result;
bool found = table_search(test_tree, "A", &result);
// assert(!found);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_insert_root, "Insert an item (H,1)")
table_init(&test_tree);
table_insert(&test_tree, "H", 1);
assert(tree_invariant(test_tree));
assert(test_tree != NULL);
assert(test_tree->value == 1);
assert(test_tree->left == NULL);
assert(test_tree->right == NULL);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_search_root, "Search in a single node tree (H)")
table_init(&test_tree);
table_insert(&test_tree, "H", 1);
int result;
bool found = table_search(test_tree, "H", &result);
assert(found);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_update_root, "Update a node in a single node tree (H,1)->(H,8)")
table_init(&test_tree);
table_insert(&test_tree, "H", 1);
bst_print_tree(test_tree);
table_insert(&test_tree, "H", 8);
assert(test_tree->value == 8);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_insert_many, "Insert many values")
table_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
assert(tree_invariant(test_tree));
assert(-1 <= balance_node(test_tree) && balance_node(test_tree) <= 1);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_search, "Search for an item deeper in the tree (A)")
table_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
int result;
bool found = table_search(test_tree, "A", &result);
// assert(result == 1);
// assert(found);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_search_missing, "Search for a missing key (X)")
table_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
int result;
bool found = table_search(test_tree, "X", &result);
assert(!found);
bst_print_tree(test_tree);
ENDTEST

TEST(test_tree_dispose_filled, "Dispose the whole tree")
table_init(&test_tree);
bst_insert_many(&test_tree, base_keys, base_values, base_data_count);
bst_print_tree(test_tree);
table_dispose(&test_tree);
assert(test_tree == NULL);
bst_print_tree(test_tree);
ENDTEST



#ifdef EXA

TEST(test_letter_count, "Count letters");
table_init(&test_tree);
letter_count(&test_tree, "abBcCc_ 123 *");
bst_print_tree(test_tree);
ENDTEST

TEST(test_balance, "Count letters and balance");
table_init(&test_tree);
letter_count(&test_tree, "abBcCc_ 123 *");
table_balance(&test_tree);
assert(tree_invariant(test_tree));
bst_print_tree(test_tree);
ENDTEST

#endif // EXA

int main(void) {
  init_test();

  test_tree_init();
  test_tree_dispose_empty();
  test_tree_search_empty();
  test_tree_insert_root();
  test_tree_search_root();
  test_tree_update_root();
  test_tree_insert_many();
  test_tree_search();
  test_tree_search_missing();
  test_tree_dispose_filled();

#ifdef EXA
  test_letter_count();
  test_balance();
#endif // EXA
}
