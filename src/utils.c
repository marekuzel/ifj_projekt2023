#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include "utils.h"
#include "errors.h"

void buffer_init(BufferT *buffer) {
    assert(buffer != NULL);

    buffer->length = 0;
    buffer->cap = BUFFER_CAP_S;

    buffer->bytes = (char *) calloc(buffer->cap,1);
    
    CHECK_MEM_ERR(buffer->bytes)
}


void buffer_append(BufferT *buffer, const char chr) {
    assert(buffer != NULL);

    if (buffer->length >= buffer->cap) {
        char *new_buff = realloc(buffer->bytes,buffer->cap * 2); 

        CHECK_MEM_ERR(new_buff)
        
        buffer->bytes = new_buff;
        buffer->cap *= 2;
    }

    buffer->bytes[buffer->length++] = chr; 
}

void buffer_clear(BufferT *buffer) {
    assert(buffer != NULL);

    buffer->length = 0;
}

char *buffer_export(BufferT *buffer) {
    assert(buffer != NULL);

    char * dst = (char * ) calloc(1,buffer->length+1);

    CHECK_MEM_ERR(dst)

    memcpy(dst,buffer->bytes,buffer->length);

    buffer_detor(buffer);
    
    return dst;
}

void buffer_detor (BufferT *buffer) {
    assert(buffer != NULL);

    free(buffer->bytes);
    buffer->bytes = NULL;
    buffer->cap = 0;
    buffer->length = 0;
}

buff_ret_t buffer_apend_hex_num(BufferT *buffer, char *num_str) {
    assert(buffer != NULL);
    assert(num_str != NULL);

    char tmp_str[4];
    char *endptr = NULL;
    errno = 0;

    int num = (int ) strtol(num_str,&endptr,16);

    if (errno != 0 || !num_str || *endptr) {
        return BUFF_NUM_CVT_FAIL;
    }
    
    sprintf(tmp_str, "%03d",num);

    for (int str_idx = 0; tmp_str[str_idx] != '\0'; str_idx++) {
        buffer_append(buffer,tmp_str[str_idx]);
    }
    return BUFF_NUM_CVT_SUCCES;

}


void Stack_Init(Stack *stack) {
    assert(stack != NULL);

	stack->array = calloc(STACK_SIZE,sizeof(TokenT*));

    CHECK_MEM_ERR(stack->array)

	stack -> topIndex = -1;
    stack->size = STACK_SIZE;
    stack -> bottomIndex = 0;
}

bool Stack_IsEmpty(const Stack *stack) {
    assert(stack != NULL);
	return stack->bottomIndex > stack->topIndex;
}

bool Stack_IsFull(const Stack *stack) {
    assert(stack != NULL);
    return stack->topIndex == stack->size-1;
}

void Stack_Top( const Stack *stack, TokenT **dataPtr ) {
    assert(stack != NULL);
    assert(dataPtr != NULL);
    
    if (!Stack_IsEmpty(stack)){
        *dataPtr = stack->array[stack->topIndex];
    }
}

TokenT* stack_read_token_bottom(Stack* stack) {
  assert(stack != NULL);    
  assert(stack->bottomIndex <= stack->topIndex);

  return stack->array[stack->bottomIndex++];
}

void Stack_Pop( Stack *stack) {
    assert(stack != NULL);

	if (!Stack_IsEmpty(stack)){
		stack->topIndex--;
	}

    fprintf(stderr,"Internal stack underflow\n");
    exit(INTERNAL_COMPILER_ERROR);
}

void Stack_Push( Stack *stack, TokenT *data ) {
    assert(stack != NULL);
    assert(data != NULL);

    if (Stack_IsFull(stack)) {
        TokenT **new_arr = realloc(stack->array,stack->size * 2);

        CHECK_MEM_ERR(new_arr)

        stack->size *= 2;
        stack->array = new_arr;
	}
    stack->array[stack->topIndex++] = data;
}

void Stack_Dispose( Stack *stack ) {
    assert(stack != NULL);

    for (int stack_idx = 0; stack_idx < stack->size; stack_idx++) {
        token_dtor(stack->array[stack_idx]);
    }

	stack->topIndex = -1;
    stack->bottomIndex = 0;
	free(stack->array);
	stack->array = NULL;
}


void token_init(TokenT *token,TokenType type, BufferT *buff) {
    assert(token != NULL);
    assert(buff != NULL);

    token->type = type;
    token->value.str = buffer_export(buff);

    if (type == TOKEN_INTEGER) { 
        token->value.i = (int) strtol(token->value.str,NULL,10);  
    } else if (type == TOKEN_DOUBLE) {
        token->value.d = strtod(token->value.str,NULL);
    }
}

void token_dtor(TokenT *token) {
    assert(token != NULL);

    if (token->type != TOKEN_INTEGER && token->type != TOKEN_DOUBLE)
        free(token->value.str);
    free(token);
}


#define STACKDEF(T, TNAME)                                                     \
  void stack_##TNAME##_init(stack_##TNAME##_t *stack) { stack->top = -1; }     \
                                                                               \
  void stack_##TNAME##_push(stack_##TNAME##_t *stack, T item) {                \
    if (stack->top == MAXSTACK - 1) {                                          \
      printf("[W] Stack overflow\n");                                          \
    } else {                                                                   \
      stack->items[++stack->top] = item;                                       \
    }                                                                          \
  }                                                                            \
                                                                               \
  T stack_##TNAME##_top(stack_##TNAME##_t *stack) {                            \
    if (stack->top == -1) {                                                    \
      return NULL;                                                             \
    }                                                                          \
    return stack->items[stack->top];                                           \
  }                                                                            \
                                                                               \
  T stack_##TNAME##_pop(stack_##TNAME##_t *stack) {                            \
    if (stack->top == -1) {                                                    \
      printf("[W] Stack underflow\n");                                         \
      return NULL;                                                             \
    }                                                                          \
    return stack->items[stack->top--];                                         \
  }                                                                            \
                                                                               \
  bool stack_##TNAME##_empty(stack_##TNAME##_t *stack) {                       \
    return stack->top == -1;                                                   \
  }                                                                            \

STACKDEF(char*, char)

bool stack_char_2oftop(stack_char_t *stack) {
  assert(stack != NULL);
  if (stack_char_empty(stack)) {
    return false;
  }  

  char* second = stack_char_top(stack);
  stack_char_pop(stack);

  if (stack_char_empty(stack)) {
    return false;
  }  

  char* first = stack_char_top(stack);

  char* result = (char*)malloc(1 + strlen(first)+ strlen(second)); // TODO free me
  strcpy(result, first);
  strcat(result, second);
  
  stack_char_push(stack, second);
  bool res = !strcmp(result, "$E");
  free(result);
  return res;
}

bool isTerminal (char* stackSymbol) {
  assert(stackSymbol != NULL);

  if (!strcmp(stackSymbol, "+")) {
        return true;
    } else if (!strcmp(stackSymbol, "-")) {
        return true;
    } else if (!strcmp(stackSymbol, "*")) {
        return true;
    } else if (!strcmp(stackSymbol, "/")) {
        return true;
    } else if (!strcmp(stackSymbol, "(")) {
        return true;
    } else if (!strcmp(stackSymbol, ")")) {
        return true;
    } else if (!strcmp(stackSymbol, "i")) {
        return true;
    } else if (!strcmp(stackSymbol, "$")) {
        return true;
    } else if (!strcmp(stackSymbol, "==")) {
        return true;
    } else if (!strcmp(stackSymbol, "!=")) {
        return true;
    } else if (!strcmp(stackSymbol, "<")) {
        return true;
    } else if (!strcmp(stackSymbol, "<=")) {
        return true;
    } else if (!strcmp(stackSymbol, ">")) {
        return true;
    } else if (!strcmp(stackSymbol, ">=")) {
        return true;
    } else if (!strcmp(stackSymbol, "??")) {
        return true;
    } else if (!strcmp(stackSymbol, "$")) {
        return true;
    } else {
      return false;
    }
}

Error stack_insertAfterTerminal(stack_char_t* stack) {
  assert(stack != NULL);

  stack_char_t tmp;
  stack_char_init(&tmp);

  while (!stack_char_empty(stack)) {
    char* top = stack_char_top(stack);

    if (isTerminal(top)) {
      stack_char_push(stack, "[");

      while (!stack_char_empty(&tmp)) {
        char* insert = stack_char_top(&tmp);
        stack_char_push(stack, insert);
        stack_char_pop(&tmp);
      }
      
      return SUCCESS;
    } else {
      stack_char_push(&tmp, top);
      stack_char_pop(stack);
    }
  }

  return SYNTAX_ERROR; // check me
}

int stack_numOfElem(stack_char_t* stack) {
  assert(stack != NULL);
  stack_char_t tmp;
  stack_char_init(&tmp);
  int count = 0;

  while (!stack_char_empty(stack)) {
    char* item = stack_char_top(stack);
    stack_char_push(&tmp, item);
    stack_char_pop(stack);
    count++;
  }

  while (!stack_char_empty(&tmp)) {
    char* item = stack_char_top(&tmp);
    stack_char_push(stack, item);
    stack_char_pop(&tmp);
  }

  return count;
}

void stack_topTerminal(stack_char_t* stack, char **term) {
  assert(stack != NULL);
  assert(term != NULL);

  stack_char_t tmp;
  stack_char_init(&tmp);

  while (!stack_char_empty(stack)) {
    char* top = stack_char_top(stack);

    if (isTerminal(top)) {
      while (!stack_char_empty(&tmp)) {
        char* insert = stack_char_top(&tmp);
        stack_char_push(stack, insert);
        stack_char_pop(&tmp);
      }
      *term = top;
      return;
    } else {
      stack_char_push(&tmp, top);
      stack_char_pop(stack);
    }
  }
}

char* stack_bottom_read(stack_char_t* stack) {
  assert(stack != NULL);
  stack_char_t tmp;
  stack_char_init(&tmp);
  char* bottom = "";

  while (!stack_char_empty(stack)) {
    bottom = stack_char_top(stack);
    stack_char_push(&tmp, bottom);
    stack_char_pop(stack); 
  }

  if (!stack_char_empty(&tmp)) {
    stack_char_pop(&tmp);
  }

  while (!stack_char_empty(&tmp)) {
    char* chr = stack_char_top(&tmp);
    stack_char_push(stack, chr);
    stack_char_pop(&tmp);
  }

  return bottom;
}

void print_stack(stack_char_t* stack) {
  assert(stack != NULL);
  
  stack_char_t tmp;
  stack_char_init(&tmp);

  while (!stack_char_empty(stack)) {
    char* item = stack_char_top(stack);
    printf("%s\n", item);
    stack_char_push(&tmp, item);
    stack_char_pop(stack);
  }

  while (!stack_char_empty(&tmp)) {
    char* insert = stack_char_top(&tmp);
    stack_char_push(stack, insert);
    stack_char_pop(&tmp);
  }
}

static const char *tokentype_to_string[] = {
    "TOKEN_ZERO",
    "TOKEN_IDENTIFIER",
    "TOKEN_DT_DOUBLE",
    "TOKEN_DT_DOUBLE_NIL",
    "TOKEN_DT_INT",
    "TOKEN_DT_INT_NIL",
    "TOKEN_DT_STRING",
    "TOKEN_DT_STRING_NIL",
    "TOKEN_LET",
    "TOKEN_VAR",
    "TOKEN_RETURN",
    "TOKEN_IF",
    "TOKEN_ELSE",
    "TOKEN_WHILE",
    "TOKEN_FUNC",
    "TOKEN_NIL",
    "TOKEN_OPERATOR",
    "TOKEN_STRING", 
    "TOKEN_INTEGER", 
    "TOKEN_DOUBLE",
    "TOKEN_ASSIGN",
    "TOKEN_L_BRACKET",
    "TOKEN_R_BRACKET",
    "TOKEN_LC_BRACKET",
    "TOKEN_RC_BRACKET",
    "TOKEN_COLON",
    "TOKEN_COMMA",
    "TOKEN_ARROW",
    "TOKEN_UNDERSCORE",
    "TOKEN_EOF"
};


void print_token(TokenT *token) {
    if (token == NULL) {
        printf("Invalid input\n");
        return;
    }
    printf("TOKEN : ");
    switch (token->type) {
    case TOKEN_INTEGER:
        printf("%s %d\n",tokentype_to_string[TOKEN_INTEGER],token->value.i);
        break;
    case TOKEN_DOUBLE:
        printf("%s %a\n",tokentype_to_string[TOKEN_DOUBLE],token->value.d);
        break;
    default:
        printf("%s %s\n",tokentype_to_string[token->type],token->value.str);
        break;
    }
}