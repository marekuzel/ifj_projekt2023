#include "stack.h"
#include "errors.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * Makro generující implementaci funkcí pracujících se zásobníky.
 * Podrobnější popis zásobníků v stack.h.
 */
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

void print_stack(stack_char_t* stack) {
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