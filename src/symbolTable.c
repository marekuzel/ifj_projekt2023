
#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
/*
TBD
 */
void table_init(symbol_table_t **table) {
    if (table == NULL)
        return;
        
    *table = NULL;
}

/*
 TBD
 */
bool table_search(symbol_table_t *table, char key, int *value) {

    //prázdny strom alebo sa prvok nenašiel
    if (table == NULL)
        return false;

    //prvok sa našiel
    if (key == table->key)
    {
        *value = table->value;
        return true;
    }

    //hľadanie vľavo
    if (key < table->key)
        return table_search(table->left,key,value);

    //hľadanie vpravo
    return table_search(table->right,key,value);
}


int get_height(symbol_table_t *table)
{
    if (table == NULL)
        return 0;
    return table->height;
}


void table_rotate_right(symbol_table_t **target)
{
    symbol_table_t *tmp = (*target)->left;
    (*target)->left = tmp->right;
    tmp->right = *target;
}


void table_rotate_left(symbol_table_t **target)
{
    symbol_table_t *tmp = (*target)->right;
    (*target)->right = tmp->left;
    tmp->left = *target;
}


int max(int a, int b)
{
    return (a > b) ? a : b;
}


int balance_node(symbol_table_t *table)
{
    if (table == NULL)
        return 0;
    
    return get_height(table->left) - get_height(table->right);
}

void balance_table(symbol_table_t **table, const char key)
{
    (*table)->height = 1 + max(get_height((*table)->left), get_height((*table)->right));
    int balance = balance_node(*table);

    if (balance > 1 && key < (*table)->left->key)
    {
        table_rotate_right(table);
    } else if ( balance < -1 && key > (*table)->right->key)
    {
        table_rotate_left(table);
    } else if (balance > 1 && key > (*table)->left->key)
    {
        table_rotate_left(&((*table)->left));
        table_rotate_right(table);
    } else if (balance < -1 && key < (*table)->right->key)
    {
        table_rotate_right(&((*table)->right));
        table_rotate_left(table);
    }
}

/*
TBD
 */
void table_insert(symbol_table_t **table, char key, int value) {

    //prázdny strom
    if (table == NULL)
        return;

    /*
    Hľadanie prvku, alebo miesta na vloženie novhé prvku
    */

    //našlo sa miesto na vloženie  
    if (*table == NULL)
    { 
        /*
        vloženie prvku
        */
       
        (*table) = malloc(sizeof(symbol_table_t));

        if (*table != NULL)
        {
            (*table)->key = key;
            (*table)->value = value;
            (*table)->left = NULL;
            (*table)->right = NULL;
            (*table)->height = 0;
        }
    }
    //hľadanie vľavo
    else if (key < (*table)->key)
    {
        table_insert(&((*table)->left),key,value);
    }
    //hľadanie vpravo
    else if (key > (*table)->key)
    {
        table_insert(&((*table)->right),key,value);
    }
    //našiel som prvok => aktualizujem hodnotu
    else if (key == (*table)->key)
    {
        (*table)->value = value;
    }
    
    balance_table(table,key);

} //table_insert()

/*
 TBD
 */
void table_replace_by_rightmost(symbol_table_t *target, symbol_table_t **table) {
    symbol_table_t *tmp; // dočasná premenná na správne uvoľnenie pamäti

    //našiel sa najpravejší prvok
    if ((*table)->right == NULL)
    {    
        //kopírovanie dát do 'target'
        target->value = (*table)->value;
        target->key = (*table)->key;

        //zmazanie najpravejšieho prvku
        tmp = *table;
        (*table) = (*table)->left;
        free(tmp);
        return;
    }

    //hľadanie najpravejšieho prvku
    table_replace_by_rightmost(target,&((*table)->right));

}

/*
 TBD
 */
void table_delete(symbol_table_t **table, char key) {

    //prázdny strom alebo sa prvok nanašiel
    if (table == NULL || *table == NULL)
        return;
    
    symbol_table_t *tmp; //dočasná premenná na správne uvoľnenie pamäti

    /*
    hľadanie prvku na zmazanie
    */

    if (key < (*table)->key)
    {//hľadanie vľavo
        table_delete(&((*table)->left),key);
    }
    else if (key > (*table)->key)
    {//hľadanie vpravo
        table_delete(&((*table)->right),key);
    }
    else if ((*table)->key == key)
    {//prvok sa našiel

        /*
        Mazanie prvku
        */

        if ((*table)->left == NULL && (*table)->right == NULL)
        {//prvok je list
            free(*table);
            *table = NULL;
        }
        else if ((*table)->left == NULL)
        {//prvok má len pravý podstrom
            tmp = *table;
            *table = (*table)->right;
            free(tmp);
        }
        else if ((*table)->right == NULL)
        {//prvok má len ľavý podstrom   
            tmp = *table;
            *table = (*table)->left;
            free(tmp);
        }
        else
        {//prvok má oba prodstromy
            table_replace_by_rightmost(*table,&((*table)->left));
        }
    } //else if

} //table_delete()

/*
 TBD
 */
void table_dispose(symbol_table_t **table) {

    //prázdny strom
    if (table == NULL || *table == NULL)
        return;

    //postorder prechod so uvoľnením prvku
    table_dispose(&((*table)->left));
    table_dispose(&((*table)->right));
    free(*table);
    *table = NULL;
}   
