/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#include "stack_cd.h"
#include <string.h>



CDStack CDinit_stack(CDStack *stack) {
    stack = malloc(sizeof(CDStack));
    stack->top = -1;
    return *stack;
}


char *CDpop(CDStack *stack) {
    if (stack->top < 0)
        return "";

    char* ret = stack->arr[stack->top];
    stack->arr[stack->top--] = "";
    return ret;
}


void CDpush(CDStack *stack, char *line) {
    strcpy(stack->arr[++stack->top], line);
    //stack->arr[++stack->top] = line;
}



char *CDtop(CDStack stack) {
    return stack.arr[stack.top];
}

