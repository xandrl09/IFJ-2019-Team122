/**
 * @author Ondřej Andrla
 * Year 2019
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "expresions.h"
#include "main.h"

/**
 * Structure list contains its type and pointer to successor.
 */
typedef struct list{
    symbol_enum symbol;
    struct list *next;
}expression_list;

/**
 * Structure representing stack.
 * Top is pointer on list on head of stack.
 */
typedef struct stack{
    expression_list *top;
}expression_stack;


void expression_stack_init(expression_stack * target);
bool expression_stack_empty(expression_stack *target);
void expression_stack_pop(expression_stack *target);
void expression_stack_push(expression_stack *target, symbol_enum data);
void insert_after_top_terminal(expression_stack* target,  symbol_enum symb);
expression_list* stack_top_terminal(expression_stack* target);
expression_list* expression_stack_top(expression_stack *target);
void expression_stack_destroy(expression_stack *target);




