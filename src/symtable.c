/**
 * Project: Compliler IFJ23 implementation 
 * File: symtable.c
 * 
 * @brief implementation of symbols table
 * 
 * @author Tomáš Zgút xzgutt00
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symtable.h"
#include "errors.h"
#include "utils.h"

/*
*******************************
Implementaion of symtalbe entry
*******************************
*/

symtable_entry_t *entry_create(void) {
    symtable_entry_t *new_entry = calloc(1,sizeof(symtable_entry_t));

    CHECK_MEM_ERR(new_entry)
        
    return new_entry;
}

void entry_dispose(symtable_entry_t *entry) {
    assert(entry != NULL);

    if (entry->params == NULL)
        return;

    for (int param_idx = 0; entry->params[param_idx] != NULL; param_idx++) {
        free(entry->params[param_idx]->id);
        free(entry->params[param_idx]->name);
        free(entry->params[param_idx]);
    }

    free(entry->params);
}


/*
*********************************************
Implementatoin of awl tree - helper functions
*********************************************
*/

int key_cmp(const char *key, const char *cmp) {
    assert(key != NULL);
    assert(cmp != NULL);

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

int get_height(awl_t *awl) {
    if (awl == NULL)
        return 0;
    return awl->height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int balance_node(awl_t *awl) {
    if (awl == NULL)
        return 0;
    
    return get_height(awl->left) - get_height(awl->right);
}


/*
**************************
Iplementation of awl tree
**************************
*/


void awl_init(awl_t **awl) {
    assert(awl != NULL);

    *awl = NULL;
}

bool awl_search(awl_t *awl, char *key, symtable_entry_t **entry) {
    assert(entry != NULL);

    if (awl == NULL)
        return false;

    if (key_cmp(key,awl->key) == 0) {
        *entry = awl->value;
        return true;
    }

    if (key_cmp(key,awl->key) == -1)
        return awl_search(awl->left,key,entry);

    return awl_search(awl->right,key,entry);
}


awl_t *awl_rotate_right(awl_t *target) {
    assert(target != NULL);

    awl_t *tmp = target->left;
    target->left = tmp->right;
    tmp->right = target;

    target->height = max(get_height(target->left), get_height(target->right)) + 1;
    tmp->height = max(get_height(tmp->left), get_height(tmp->right)) + 1;
    return tmp;
}


awl_t *awl_rotate_left(awl_t *target) {
    assert(target != NULL);

    awl_t *tmp = target->right;
    target->right = tmp->left;
    tmp->left = target;

    target->height = max(get_height(target->left), get_height(target->right)) + 1;
    tmp->height = max(get_height(tmp->left), get_height(tmp->right)) + 1;
    return tmp;
}

void awl_balance(awl_t **awl, const char *key) {
    assert(awl != NULL);
    assert(key != NULL);

    (*awl)->height = 1 + max(get_height((*awl)->left), get_height((*awl)->right));
    int balance = balance_node(*awl);

    if (balance > 1 && key_cmp(key,(*awl)->left->key) == -1) {
        (*awl) = awl_rotate_right(*awl);
    } else if ( balance < -1 && key_cmp(key,(*awl)->right->key) == 1) {
        (*awl) = awl_rotate_left(*awl);
    } else if (balance > 1 && key_cmp(key,(*awl)->left->key) == 1) {
        (*awl)->left = awl_rotate_left((*awl)->left);
        (*awl) = awl_rotate_right(*awl);
    } else if (balance < -1 && key_cmp(key,(*awl)->right->key) == 1) {
        (*awl)->right = awl_rotate_right((*awl)->right);
        *awl = awl_rotate_left(*awl);
    }
}


void awl_insert(awl_t **awl, char *key, symtable_entry_t *entry) {
    assert(awl != NULL);
    assert(entry != NULL);

    if (*awl == NULL) { 

        (*awl) = calloc(1,sizeof(awl_t));

        CHECK_MEM_ERR(*awl)


        (*awl)->key = add_string(key);
        (*awl)->value = entry;
        (*awl)->left = NULL;
        (*awl)->right = NULL;
        (*awl)->height = 0;
        return;

    } else if (key_cmp(key,(*awl)->key) == -1) {
        awl_insert(&((*awl)->left),key,entry);
    } else if (key_cmp(key,(*awl)->key) == 1) {
        awl_insert(&((*awl)->right),key,entry);
    } else if (key_cmp(key,(*awl)->key) == 0) {
        (*awl)->value = entry;
    }
    
    awl_balance(awl,key);

} //awl_insert()


void awl_dispose(awl_t **awl) {
    if (awl == NULL || *awl == NULL)
        return;

    awl_dispose(&((*awl)->left));
    awl_dispose(&((*awl)->right));
    free((*awl)->key);
    entry_dispose((*awl)->value);
    free((*awl)->value);
    free(*awl);
    *awl = NULL;
}   

/*
***************************
Implelentation of symtable
***************************
*/

void table_init(symtable_t *table) {
    assert(table != NULL);

    awl_t **new_tree_array = calloc(SYMTABLE_SIZE,sizeof(awl_t*));

    CHECK_MEM_ERR(new_tree_array)

    table->table_stack = new_tree_array;
    table->size = SYMTABLE_SIZE;
    table->top_idx = -1;
    table_add_scope(table);
    table_insert_builtin_funcs(table);
}


void table_add_scope(symtable_t *table) {
    assert(table != NULL);

    if (table->top_idx >= SYMTABLE_SIZE - 1) {
        fprintf(stderr,"Symtable scope overflow\n");
        exit(INTERNAL_COMPILER_ERROR);
    }

    table->top_idx++;
    awl_init(&(table->table_stack[table->top_idx]));
}

void table_remove_scope(symtable_t *table) {
    assert(table != NULL);

    if (table->top_idx < 0) {
        fprintf(stderr,"Symtable scope underflow\n");
        exit(INTERNAL_COMPILER_ERROR);
    }

    awl_dispose(&(table->table_stack[table->top_idx]));
    table->top_idx--;
}

void table_insert(symtable_t *table, char *key, symtable_entry_t **entry) {
    assert(table != NULL);
    assert(key != NULL);
    assert(entry != NULL);

    symtable_entry_t *new_entry = entry_create();
    symtable_entry_t *tmp_entry;
    awl_insert(&(table->table_stack[table->top_idx]), key, new_entry);
    table->top_idx--;
    if (table_search(table,key,&tmp_entry)) {
        new_entry->redeclared = true;
    }
    table->top_idx++;
    *entry = new_entry;
}

void table_insert_global(symtable_t *table, char *key, symtable_entry_t **entry) {
    assert(table != NULL);
    assert(key != NULL);
    assert(entry != NULL);

    symtable_entry_t *new_entry = entry_create();
    awl_insert(&(table->table_stack[0]), key, new_entry);
    *entry = new_entry;
}

void table_function_insert(symtable_t *table, char *key, param_t **params, TokenType return_type) {
    assert(table != NULL);
    assert(key != NULL);

    symtable_entry_t *entry;
    table_insert_global(table, key, &entry);
    entry->type = TOKEN_FUNC;
    entry->params = params;
    entry->return_type = return_type;
}

bool table_search(symtable_t *table, char *key, symtable_entry_t **entry) {
    assert(table != NULL);
    assert(key != NULL);
    assert(entry != NULL);

    bool found = false;
    int table_idx = table->top_idx == -1 ? 0 : table->top_idx;
    while(table_idx >= 0 && !found) {
        found = awl_search(table->table_stack[table_idx],key,entry);
        table_idx--;
    }
    return found;
}

bool table_search_global(symtable_t *table, char *key, symtable_entry_t **entry) {
    assert(table != NULL);
    assert(key != NULL);
    assert(entry != NULL);

    return awl_search(table->table_stack[0],key,entry);
}

void table_dispose(symtable_t *table) {
    assert(table != NULL);

    for(int i =0; table->table_stack[i] != NULL; i++)
        awl_dispose(&(table->table_stack[i]));

    free(table->table_stack);
    table->size = 0;
    table->top_idx = -1;
}


void awl_traverse(awl_t* awl, action_t action){
    assert(action != NULL);

    if(awl == NULL)
        return; 

    action(awl);
    awl_traverse(awl->left,action);
    awl_traverse(awl->right,action);
}

void table_traverse(symtable_t *table, action_t action) {
    assert(table != NULL);
    assert(action != NULL);

    for (int table_idx = table->top_idx; table_idx >= 1; table_idx--)
        awl_traverse(table->table_stack[table_idx],action);
}



/*
*********************************
Implementation of parram buffer
*********************************
*/

void param_buffer_init(ParamBufferT *buffer) {
    assert(buffer != NULL);

    buffer->length = 0;
    buffer->cap = BUFFER_CAP_S;

    buffer->bytes = calloc(buffer->cap,sizeof(param_t*));
    
    CHECK_MEM_ERR(buffer->bytes)
}


buff_ret_t table_insert_param(ParamBufferT *buffer, param_t *param) {
    assert(buffer != NULL);
    assert(param != NULL);

    if (buffer->length >= buffer->cap) {
        param_t **new_buff = realloc(buffer->bytes,buffer->cap * 2); 

        CHECK_MEM_ERR(new_buff)
        
        buffer->bytes = new_buff;
        buffer->cap *= 2;
    }
    for (int idx = 0; idx < buffer->length; idx ++) {
        if (strcmp(param->id,buffer->bytes[idx]->id) == 0 ||
        (strcmp(param->name,buffer->bytes[idx]->name) == 0 && strcmp(param->name,"_") != 0)) {
            return BUFF_APPEND_FAIL;
        }

    }
    buffer->bytes[buffer->length++] = param;
    return BUFF_APPEND_SUCCES;  

}

param_t **param_buffer_export(ParamBufferT *buffer) {
    assert(buffer != NULL);

    param_t **dst = calloc(sizeof(param_t*),buffer->length+1);
    
    CHECK_MEM_ERR(dst)

    memcpy(dst,buffer->bytes,buffer->length * sizeof(param_t *));

    buffer->length = 0;
    return dst;
}

void param_buffer_detor(ParamBufferT *buffer) {
    assert(buffer != NULL);

    free(buffer->bytes);
    buffer->bytes = NULL;
    buffer->cap = 0;
    buffer->length = 0;
}


void param_value_init(symtable_t *table, param_t *param, litValue value, TokenType type) {
    assert(table != NULL);
    assert(param != NULL);

    switch (type)
    {
    case TOKEN_INTEGER:
        param->value.i = value.i; 
        break;

    case TOKEN_DOUBLE:
        param->value.d = value.d; 
        break;

    case TOKEN_NIL:
        break;

    case TOKEN_STRING:
        param->value.str = value.str;
        break;

    default:
        param->value.str = value.str;
        param->var = true;
        param->var = true;
        param->global = is_global(table,value.str);
        break;
    }
}


param_t *param_create(char *id, char *name, TokenType type) {
    assert(name != NULL);
    assert(id != NULL);

    param_t *new_param = calloc(1,sizeof(param_t));

    CHECK_MEM_ERR(new_param)

    new_param->id = add_string(id);
    new_param->name = add_string(name);
    new_param->type = type;

    return new_param;
}

void insert_builtin(symtable_t *table, char *name, TokenType ret_type, param_t **params, int param_num) {
    assert(table != NULL);
    assert(name != NULL);

    param_t **new_params = NULL;

    if (param_num > 0) {
        new_params = calloc(param_num + 1, sizeof(param_t *));

        CHECK_MEM_ERR(new_params)

        memcpy(new_params,params,sizeof(param_t*)*param_num);
    }
    table_function_insert(table,name,new_params,ret_type);
    
}

void table_insert_builtin_funcs(symtable_t *table) {
    assert(table != NULL);

    param_t *params[3];

    /**readString*/
    insert_builtin(table,"readString",TOKEN_DT_STRING,NULL,0);

    /**readDouble*/
    insert_builtin(table,"readDouble",TOKEN_DT_DOUBLE,NULL,0);

    /**readInt*/
    insert_builtin(table,"readInt",TOKEN_DT_INT,NULL,0);

    /**write*/
    params[0] = param_create("_", "_", TOKEN_ZERO);
    insert_builtin(table,"write",TOKEN_NIL,params,1);

    /**Int2Double*/
    params[0] = param_create("term", "_", TOKEN_DT_INT);
    insert_builtin(table,"Int2Double",TOKEN_DT_DOUBLE,params,1);

    /**Double2Int*/
    params[0] = param_create("term", "_", TOKEN_DT_DOUBLE);
    insert_builtin(table,"Double2Int",TOKEN_DT_INT,params,1);

    /**length*/
    params[0] = param_create("s", "_", TOKEN_DT_STRING);
    insert_builtin(table,"length",TOKEN_DT_INT,params,1);

    /**ord*/
    params[0] = param_create("c","_",TOKEN_DT_STRING);
    insert_builtin(table, "ord", TOKEN_DT_INT, params,1);

    /*chr*/
    params[0] = param_create("i","_",TOKEN_DT_INT);
    insert_builtin(table,"chr", TOKEN_DT_STRING,params,1);

    /**susbstring*/
    params[0] = param_create("s", "of", TOKEN_DT_STRING);
    params[1] = param_create("i", "startingAt", TOKEN_DT_INT);
    params[2] = param_create("j", "endingBefore", TOKEN_DT_INT);
    insert_builtin(table,"substring",TOKEN_DT_STRING,params,3);

}

bool is_global(symtable_t *table, char *name) {
    assert(table != NULL);
    assert(name);

    symtable_entry_t *entry;
    return table_search_global(table,name,&entry);
}


char* add_string(char *str) {
    assert(str != NULL);

    size_t str_len = strlen(str);
    char *new_str = calloc(str_len+1,1);

    CHECK_MEM_ERR(new_str)

    memcpy(new_str,str,str_len);
    return new_str;
}


void add_params_to_scope(symtable_t *table, symtable_entry_t *entry) {
    assert(table != NULL);
    assert(entry != NULL);
    
    symtable_entry_t *tmp;
    for (int param_idx = 0; entry->params[param_idx] != NULL; param_idx++) {
        table_insert(table,entry->params[param_idx]->id,&tmp);
    }
}
