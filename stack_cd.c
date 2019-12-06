/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#include "stack_cd.h"
#include "err.h"
#include <string.h>
#include <stdio.h>


CDStack * CDinit_stack() {
    CDStack *stack = malloc(sizeof (CDStack));
    stack->top = NULL;
    stack->size = 0;
    return stack;
}

char *copy_string(char *str)    {
    char *tmp = malloc(strlen(str) + 1);
    if (tmp)
        strcpy(tmp, str);
    return tmp;
}


char *CDpop(CDStack *stack)     {
    if (stack->top != NULL) {
        CDStack_element *tmp = stack->top;
        stack->top = stack->top->next;
        free(tmp->value);
        stack->size--;
    }
}


void CDpush(CDStack *stack, char *line) {
    CDStack_element *elem = malloc(sizeof(elem));
    if (elem)   {
        elem->value = copy_string(line);
        elem->next = stack->top;
        stack->top = elem;
        stack->size++;
    }else
        errInter();
}


void CDStack_free_elems(CDStack *stack) {
    while(NULL != stack->top)
        CDpop(stack);
}

void CDStack_destroy(CDStack *stack) {
    CDStack_free_elems(stack);
    free(stack);
    stack = NULL;
}

char *CDtop(CDStack stack) {
    if(stack.top != NULL)
        return stack.top->value;
    else
        fprintf(stderr, "CDStack's head is NULL\n");
    return NULL;
}

