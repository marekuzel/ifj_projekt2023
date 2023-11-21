#include <stdbool.h>
#include "errors.h"

#ifndef IFJ___STACK
#define IFJ___STACK

#define MAXSTACK 30 //  max stack size



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

bool stack_char_2oftop(stack_char_t *stack);
Error stack_insertAfterTerminal(stack_char_t* stack);
int stack_numOfElem(stack_char_t* stack);
void stack_topTerminal(stack_char_t* stack, char **term);
void print_stack(stack_char_t* stack);
#endif
