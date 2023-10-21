#include <stdbool.h>

#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Buffet ADT implementation
*/
#define BUFFER_CAP_S 256
typedef struct buffer_t {
    char *bytes;
    int cap;
    int length;
} BufferT;


typedef enum {
    BUFF_INIT_FAIL,
    BUFF_INIT_SUCCES,
    BUFF_APPEND_SUCCES,
    BUFF_APPEND_FAIL
    } buff_ret_t;


/**
 * Initilaize ADT buffer
*/
buff_ret_t buffer_init(BufferT *buffer);

/**
 * Appends ch to the end of buffer
*/
buff_ret_t buffer_append(BufferT *buffer,const char chr);

/**
 * Clears buffer 
*/
void buffer_clear(BufferT *buffer);


/**
 * Exports data in buffer and returns pointer to it, to be freed
*/
char *buffer_export(const BufferT *buffer);

/**
 * Properly cleans up buffer fro memory
*/
void buffer_detor (BufferT *buffer);

/**
 * @brief Stack impementation
 * 
 */

#define STACK_SIZE 100 

typedef struct {
	char *array;
	int topIndex;
} Stack;

typedef enum {
    STACK_INIT_SUCCES,
    STACK_INIT_FAIL,
    STACK_POP_SUCCES,
    STACK_POP_FAIL,
    STACK_PUSH_SUCCES,
    STACK_PUSH_FAIL,
    } stack_ret_t;


stack_ret_t Stack_Init( Stack * );

bool Stack_IsEmpty( const Stack * );

bool Stack_IsFull( const Stack * );

stack_ret_t Stack_Pop( Stack *, char * );

stack_ret_t Stack_Push( Stack *, char );

void Stack_Dispose( Stack * );

/**
 * @brief ADT Token implementation
*/


typedef union {
    char* str;
    double d;
    int i;
} tokenValue;

typedef enum tokentype_e {
    TOKEN_ZERO, // lze na nej inicializovat token
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_DATATYPE,
    TOKEN_STRING, 
    TOKEN_INTEGER, 
    TOKEN_DECIMAL,
    TOKEN_TERM,
    TOKEN_LINE_COMMENT,
    TOKEN_BLOCK_COMMENT,
    TOKEN_EOF
} TokenType;


typedef enum {
    INT_CONVERSION_FAIL,
    INT_CONVERSION_SUCCES,
    DOUBLE_CONVERTION_FAIL,
    DOUBLE_CONVERTION_SUCCES,
    VALUE_ASSIGNMENT_FAIL,
    VALUE_ASSIGNMENT_SUCCES,
} token_ret_t;
typedef struct {
    TokenType type;
    tokenValue value;
} TokenT;



#endif