#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
#include "errors.h"

buff_ret_t buffer_init(BufferT *buffer) {
    buffer->length = 0;
    buffer->cap = BUFFER_CAP_S;

    buffer->bytes = (char *) calloc(buffer->cap,1);
    
    if (buffer->bytes == NULL) 
        return BUFF_INIT_FAIL;

    return BUFF_INIT_SUCCES;
}


buff_ret_t buffer_append(BufferT *buffer, const char chr) {
    if (buffer->length >= buffer->cap) {
        char *new_buff = realloc(buffer->bytes,buffer->cap * 2); 

        if (new_buff == NULL)
        {
            buffer_detor(buffer);
            return BUFF_APPEND_FAIL;
        }
        
        buffer->bytes = new_buff;
        buffer->cap *= 2;
    }

    buffer->bytes[buffer->length++] = chr;
    return BUFF_APPEND_SUCCES;  

}

void buffer_clear(BufferT *buffer) {
    buffer->length = 0;
}

char *buffer_export(const BufferT *buffer) {
    char * dst = (char * ) calloc(1,buffer->length+1);

    if (dst == NULL)
        return NULL;

    memcpy(dst,buffer->bytes,buffer->length);
    
    return dst;
}

void buffer_detor (BufferT *buffer) {
    free(buffer->bytes);
    buffer->bytes = NULL;
    buffer->cap = 0;
    buffer->length = 0;
}



stack_ret_t Stack_Init(Stack *stack) {
	if (stack == NULL){
		return STACK_INIT_FAIL;
	}
	stack->array = calloc(STACK_SIZE,sizeof(TokenT));
	if (stack->array == NULL){
		return STACK_INIT_FAIL;
	}
	stack -> topIndex = -1;
    return STACK_INIT_SUCCES;
}

bool Stack_IsEmpty(const Stack *stack) {
	return stack->topIndex == -1;
}

bool Stack_IsFull(const Stack *stack) {
		return stack->topIndex == STACK_SIZE-1;
}

void Stack_Top( const Stack *stack, TokenT **dataPtr ) {
    if (!Stack_IsEmpty(stack)){
        *dataPtr = stack->array[stack->topIndex];
    }
}

stack_ret_t Stack_Pop( Stack *stack, TokenT *token) {
	if (!Stack_IsEmpty(stack)){
		stack->topIndex--;
        return STACK_POP_SUCCES;
	}
    return STACK_POP_FAIL;
}

stack_ret_t Stack_Push( Stack *stack, TokenT *data ) {
	if (!Stack_IsFull(stack)) {
		stack->topIndex++;
		stack->array[stack->topIndex] = data;
        return STACK_PUSH_SUCCES;
	}
	else{
		return STACK_PUSH_FAIL;
	}
}

void Stack_Dispose( Stack *stack ) {
	stack->topIndex = -1;
	free(stack->array);
	stack->array = NULL;
}


token_ret_t token_init(TokenT *token,TokenType type, BufferT *buff) {
    token->type = type;
    char *token_value = buffer_export(buff);

    if (token_value == NULL) {
        return VALUE_ASSIGNMENT_FAIL;
    }

    if (type == TOKEN_INTEGER) {
        token->value.i = (int) strtol(token_value,NULL,10);
        return INT_CONVERSION_SUCCES;

    }
    else if (type == TOKEN_DECIMAL) {
        token->value.d = strtod(token_value,NULL);
        return DOUBLE_CONVERTION_SUCCES;
    }

    buffer_detor(buff);

    token->value.str = token_value;
    return VALUE_ASSIGNMENT_SUCCES;

}

void print_Token(TokenT *token){
    switch(token->type) {
        case TOKEN_INTEGER:
            printf("%d\n",token->value.i);
            break;
        case TOKEN_DECIMAL:
            printf("%f\n",token->value.d);
            break;
        default:
            printf("%s\n",token->value.str);
            break;
    }
}

void token_dtor(TokenT *token) {
    free(token->value.str);
    token->value.str = NULL;
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

  return !strcmp(result, "$E");
}

bool isTerminal (char* stackSymbol) {
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
  stack_char_t tmp;
  stack_char_init(&tmp);

  while (!stack_char_empty(stack)) {
    char* top = stack_char_top(stack);

    if (isTerminal(top)) {
      stack_char_push(stack, "<");

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

// void print_stack(stack_char_t* stack) {
//   stack_char_t tmp;
//   stack_char_init(&tmp);

//   while (!stack_char_empty(stack)) {
//     char* item = stack_char_top(stack);
//     printf("%s\n", item);
//     stack_char_push(&tmp, item);
//     stack_char_pop(stack);
//   }

//   while (!stack_char_empty(&tmp)) {
//     char* insert = stack_char_top(&tmp);
//     stack_char_push(stack, insert);
//     stack_char_pop(&tmp);
//   }
// }