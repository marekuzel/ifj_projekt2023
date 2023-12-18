/**
 * Project: Compliler IFJ23 implementation 
 * File: utils.h
 * 
 * @brief utilities interface for all project files 
 * 
 * @authors Tomáš Zgút xzgutt00
 *          Markéta Belatková xbelat02
 *          Tímea Adamčíková xadamc09
 *          Marek Kužel xkuzel11 
*/

#include <stdbool.h>
#include "errors.h"
#ifndef UTILS_H
#define UTILS_H


#define CHECK_MEM_ERR(ptr)                              \
    if (ptr == NULL) {                                  \
        fprintf(stderr,"Failed to allocate memory!\n"); \
        exit(INTERNAL_COMPILER_ERROR);                  \
    }                                                   \

// #define NDEBUG

/**
 * @brief Buffet ADT implementation
*/
#define BUFFER_CAP_S 256
typedef struct buffer_t {
    char *bytes;
    int cap;
    int length;
} BufferT;


typedef enum buffer_ret{
    BUFF_INIT_FAIL,
    BUFF_INIT_SUCCES,
    BUFF_APPEND_SUCCES,
    BUFF_APPEND_FAIL,
    BUFF_NUM_CVT_FAIL,
    BUFF_NUM_CVT_SUCCES
    } buff_ret_t;


/**
 * @brief Initilaize ADT buffer
 * 
 * @param buffer pointer to buffer
 * @retval ret_t BUFF_INIT_SUCCES if succesfull
 * @retval ret_t BUFF_INIT_FAIL if failed
 */
void buffer_init(BufferT *buffer);

/**
 * @brief Appends chr to the end of buffer 
 * 
 * @param buffer pointer to buffer
 * @param chr character to be appended
 * @retval ret_t BUFF_APPEND_SUCCES if succesfull
 * @retval ret_t BUFF_APPEND_FAIL if failed
 */

void buffer_append(BufferT *buffer,const char chr);

/**
 * @brief Clears buffer
 * 
 * @param buffer pointer to buffer
 */
void buffer_clear(BufferT *buffer);


/**
 * @brief exports buffer to char* (chars needs to be freed after use)
 * 
 * @param buffer pointer to buffer
 * @retval char* pointer to char array
 */
char *buffer_export(BufferT *buffer);

/**
 * @brief frees buffer from memory
 * 
 * @param buffer pointer to buffer
 */
void buffer_detor (BufferT *buffer);

/**
 * @brief converts a hex number string into decimal number string
 * and appends it to the buffer
*/

buff_ret_t buffer_apend_hex_num(BufferT *buffer, char *num_str);

/**
 * @brief takes a character and converts into ascii escape sequence
 * and appends it to the buffer.
*/
buff_ret_t buffer_append_ascii(BufferT *buffer, char ascii_code);


void convert_multilne_strings(BufferT *buffer);
typedef union {
    char* str;
    double d;
    int i;
} litValue;

typedef enum tokentype_e {
    TOKEN_ZERO,
    TOKEN_IDENTIFIER,
    
    TOKEN_DT_DOUBLE,
    TOKEN_DT_DOUBLE_NIL,
    TOKEN_DT_INT,
    TOKEN_DT_INT_NIL,
    TOKEN_DT_STRING,
    TOKEN_DT_STRING_NIL,

    TOKEN_LET,
    TOKEN_VAR,

    TOKEN_RETURN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FUNC,
    TOKEN_NIL,

    TOKEN_OPERATOR,

    TOKEN_STRING, 
    TOKEN_INTEGER, 
    TOKEN_DOUBLE,

    TOKEN_ASSIGN,
    TOKEN_L_BRACKET,
    TOKEN_R_BRACKET,
    TOKEN_LC_BRACKET,
    TOKEN_RC_BRACKET,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_ARROW, //->
    TOKEN_UNDERSCORE,

    TOKEN_EOF
} TokenType;

typedef enum token_ret{
    INT_CONVERSION_SUCCES,
    DOUBLE_CONVERTION_SUCCES,
    VALUE_ASSIGNMENT_FAIL,
    VALUE_ASSIGNMENT_SUCCES,
} token_ret_t;

typedef struct token{
    TokenType type;
    litValue value;
} TokenT;

/**
 * @brief Stack impementation
 * 
 */

#define STACK_SIZE 100 

typedef struct stack{
	TokenT **array;
	int topIndex;
    int bottomIndex;
    int size;
} Stack;

/**
 * @brief Initialize stack
 * 
 * @param Stack pointer to stack
 * @retval stack_ret_t STACK_INIT_SUCCES succesfull initialization of stack sturcture
 * @retval stack_ret_t STACK_INIT_FAIL failed initialization of stack structure
 */
void Stack_Init(Stack *);

/**
 * @brief If stack is empty return true, returns false otherwise
 * 
 * @param stack pointer to stack
 * @retval true if empty
 * @retval false if not empty
 */
bool Stack_IsEmpty(const Stack *);

/**
 * @brief Returns true if stack is full, false otherwise
 * @param stack pointer to stack
 * @retval true if full
 * @retval false if not full
 */
bool Stack_IsFull(const Stack *);

/**
 * @brief Assigns value of the top element of stack to dataPtr
 * 
 * @param stack pointer to stack
 * @param dataPtr pointer to TokenT
 */
void Stack_Top(const Stack *, TokenT **);

TokenT* stack_read_token_bottom(Stack* stack);

/**
 * @brief Pops element from stack
 * 
 * @param stack pointer to stack
 * @retval stack_ret_t STACK_POP_SUCCES if succesfull
 * @retval stack_ret_t STACK_POP_FAIL if failed
 */
void Stack_Pop(Stack *);

/**
 * @brief Stack push operation
 * 
 * @param stack pointer to stack
 * @param TokenT token to be pushed
 * @return stack_ret_t STACK_PUSH_SUCCES if succesfull
 * @return stack_ret_t STACK_PUSH_FAIL if failed
 */

void Stack_Push(Stack *, TokenT*);


/**
 * @brief Cleans up stack from memory
 * 
 * @param stack pointer to stack
 */
void Stack_Dispose(Stack *);

/**
 * @brief ADT Token implementation
*/

/**
 * @brief TokenT initialization
 * 
 * @param token: pointer to a token
 * @param type: type of token
 * @param buff: pointer to a buffer
*/
void token_init(TokenT *token,TokenType type, BufferT *buff);


/**
 * @brief Clears token from memory
 * 
 * @param token: pointer to a token
*/
void token_dtor(TokenT *token);

#define MAXSTACK 100

#define STACKDEC(T, TNAME)                                                     \
  typedef struct {                                                             \
    T items[MAXSTACK];                                                         \
    int top;                                                                   \
  } stack_##TNAME##_t;                                                         \
                                                                               \
  void stack_##TNAME##_init(stack_##TNAME##_t *stack);                         \
  void stack_##TNAME##_push(stack_##TNAME##_t *stack, T item);                 \
  T stack_##TNAME##_pop(stack_##TNAME##_t *stack);                             \
  T stack_##TNAME##_top(stack_##TNAME##_t *stack);                             \
  bool stack_##TNAME##_empty(stack_##TNAME##_t *stack);                        \

STACKDEC(char*, char)
/**
 * @brief compare top 2 strings from stack
 * 
 * @param stack pointer to stack
 * @return true if on top is end of expresion
 * @return false otherwise
*/
bool stack_char_2oftop(stack_char_t *stack);

/**
 * @brief insert after term on stack
 * 
 * @param stack pointer to stack
 * @return SUCCESS when terminal found and item was added
 * @return SYNTAX ERROR otherwise
*/
Error stack_insertAfterTerminal(stack_char_t* stack);

/**
 * @brief count of elements in stack
 * 
 * @param stack pointer to stack
 * @return number of elements
*/
int stack_numOfElem(stack_char_t* stack);

/**
 * @brief search for first term
 * 
 * @param stack pointer to stack
 * @param term first term
*/
void stack_topTerminal(stack_char_t* stack, char **term);

/**
 * @brief read from the bottom of stack
 * 
 * @param stack pointer to stack
 * @return string at the bottom of stack
*/
char* stack_bottom_read(stack_char_t* stack);

/**
 * @brief print elements of stack
 * 
 * @param stack pointer to stack
*/
void print_stack(stack_char_t* stack);

/**
 * @brief print token values
 * 
 * @param token pointer to token
*/
void print_token(TokenT *token);
extern const char *tokentype_to_string[30];

bool is_token_data_type(TokenT *token);
#endif