
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "utils.h"

/**
 * 
*/
int key_cmp(const char *key, const char *cmp) {
    int key_idx;
    for (key_idx = 0;key[key_idx] != '\0'; key_idx++) {
        if (key[key_idx] == cmp[key_idx])
            continue;

        if (cmp[key_idx] == '\0')
            return 1;

        return key[key_idx] > cmp[key_idx] ? 1 : -1;
    }

    return cmp[key_idx] == '\0' ? 0 : -1;
}


/*
TBD
 */
void table_init(symtable_t **table) {
    if (table == NULL)
        return;
        
    *table = NULL;
}

/*
 TBD
 */
bool table_search(symtable_t *table, char *key, int *value) {

    //prázdny strom alebo sa prvok nenašiel
    if (table == NULL)
        return false;

    //prvok sa našiel
    if (key_cmp(key,table->key) == 0) {
        *value = table->value;
        return true;
    }

    //hľadanie vľavo
    if (key_cmp(key,table->key) == -1)
        return table_search(table->left,key,value);

    //hľadanie vpravo
    return table_search(table->right,key,value);
}


int get_height(symtable_t *table) {
    if (table == NULL)
        return 0;
    return table->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

symtable_t *table_rotate_right(symtable_t *target) {
    symtable_t *tmp = target->left;
    target->left = tmp->right;
    tmp->right = target;

    target->height = max(get_height(target->left), get_height(target->right)) + 1;
    tmp->height = max(get_height(tmp->left), get_height(tmp->right)) + 1;
    return tmp;
}


symtable_t *table_rotate_left(symtable_t *target) {
    symtable_t *tmp = target->right;
    target->right = tmp->left;
    tmp->left = target;

    target->height = max(get_height(target->left), get_height(target->right)) + 1;
    tmp->height = max(get_height(tmp->left), get_height(tmp->right)) + 1;
    return tmp;
}


int balance_node(symtable_t *table) {
    if (table == NULL)
        return 0;
    
    return get_height(table->left) - get_height(table->right);
}

void table_balance(symtable_t **table, const char *key) {
    (*table)->height = 1 + max(get_height((*table)->left), get_height((*table)->right));
    int balance = balance_node(*table);

    if (balance > 1 && key_cmp(key,(*table)->left->key) == -1) {
        (*table) = table_rotate_right(*table);
    } else if ( balance < -1 && key_cmp(key,(*table)->right->key) == 1) {
        (*table) = table_rotate_left(*table);
    } else if (balance > 1 && key_cmp(key,(*table)->left->key) == 1) {
        (*table)->left = table_rotate_left((*table)->left);
        (*table) = table_rotate_right(*table);
    } else if (balance < -1 && key_cmp(key,(*table)->right->key) == 1) {
        (*table)->right = table_rotate_right((*table)->right);
        *table = table_rotate_left(*table);
    }
}

/*
TBD
 */
void table_insert(symtable_t **table, char *key, int value) {

    //prázdny strom
    if (table == NULL)
        return;

    /*
    Hľadanie prvku, alebo miesta na vloženie novhé prvku
    */

    //našlo sa miesto na vloženie  
    if (*table == NULL) { 
        /*
        vloženie prvku
        */
       
        (*table) = malloc(sizeof(symtable_t));
        if (*table != NULL) {
            char *tmp = calloc(30,sizeof(char));  
            sprintf(tmp,"%s",key);
            (*table)->key = tmp;
            (*table)->value = value;
            (*table)->left = NULL;
            (*table)->right = NULL;
            (*table)->height = 0;
        }
        return;
    }
    //hľadanie vľavo
    else if (key_cmp(key,(*table)->key) == -1) {
        table_insert(&((*table)->left),key,value);
    }
    //hľadanie vpravo
    else if (key_cmp(key,(*table)->key) == 1) {
        table_insert(&((*table)->right),key,value);
    }
    //našiel som prvok => aktualizujem hodnotu
    else if (key_cmp(key,(*table)->key) == 0) {
        (*table)->value = value;
    }
    
    table_balance(table,key);

} //table_insert()


/*
 TBD
 */
void table_dispose(symtable_t **table) {

    //prázdny strom
    if (table == NULL || *table == NULL)
        return;

    //postorder prechod so uvoľnením prvku
    table_dispose(&((*table)->left));
    table_dispose(&((*table)->right));
    free((*table)->key);
    //free((*table)->value);
    free(*table);
    *table = NULL;
}   
