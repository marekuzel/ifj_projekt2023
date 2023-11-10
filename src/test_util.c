#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

const char *subtree_prefix = "  |";
const char *space_prefix = "   ";

char *make_prefix(char *prefix, const char *suffix) {
  char *result = (char *)malloc(strlen(prefix) + strlen(suffix) + 1);
  strcpy(result, prefix);
  result = strcat(result, suffix);
  return result;
}
void bst_print_node(symtable_t *node) {
  printf("[%s,%d]", node->key, node->value);
}

void bst_print_subtree(symtable_t *tree, char *prefix, direction_t from) {
  if (tree != NULL) {
    char *current_subtree_prefix = make_prefix(prefix, subtree_prefix);
    char *current_space_prefix = make_prefix(prefix, space_prefix);

    if (from == left) {
      printf("%s\n", current_subtree_prefix);
    }

    bst_print_subtree(
        tree->right,
        from == left ? current_subtree_prefix : current_space_prefix, right);

    printf("%s  +-", prefix);
    bst_print_node(tree);
    printf("\n");

    bst_print_subtree(
        tree->left,
        from == right ? current_subtree_prefix : current_space_prefix, left);

    if (from == right) {
      printf("%s\n", current_subtree_prefix);
    }

    free(current_space_prefix);
    free(current_subtree_prefix);
  }
}

void bst_print_tree(symtable_t *tree) {
  printf("Binary tree structure:\n");
  printf("\n");
  if (tree != NULL) {
    bst_print_subtree(tree, "", none);
  } else {
    printf("Tree is empty\n");
  }
  printf("\n");
}


void bst_insert_many(symtable_t **tree, const char *keys[], const int values[],
                     int count) {
  for (int i = 0; i < count; i++) {
    table_insert(tree, keys[i], values[i]);
  }
}

char *maxValue(symtable_t* node)
{
    if (node == NULL)
        return "";

    char *leftMax = maxValue(node->left);
    char *rightMax = maxValue(node->right);
    char *value;
    if (key_cmp(leftMax, rightMax) == 1) {
        value = leftMax;
    }
    else {
        value = rightMax;
    }
 
    if (key_cmp(value, node->key) == -1) {
        value = node->key;
    }
    return value;
}


char *minValue(symtable_t* node)
{ 
    if (node == NULL)
        return "";

    char *leftMax = minValue(node->left);
    char *rightMax = minValue(node->right);
 
    char *value;
    if (key_cmp(leftMax,rightMax) == -1) {
        value = leftMax;
    }
    else {
        value = rightMax;
    }
 
    if (key_cmp(value, node->key) == 1 || *value == '\0' ) {
        value = node->key;
    }
    return value;
}

bool tree_invariant(symtable_t *tree)
{
    if (tree == NULL)
        return true;

    if(tree->left != NULL && key_cmp(maxValue(tree->left),tree->key) == 1)
        return false;
        
    if (tree->right != NULL && key_cmp(minValue(tree->right), tree->key) == -1) 
        return false;

    if (!tree_invariant(tree->left) ||  !tree_invariant(tree->right))
        return false;

    return true;
}
