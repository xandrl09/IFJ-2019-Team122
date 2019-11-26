/**
 * @author Ondřej Andrla
 * Year 2019
 */

#include "stack_ex.h"

/**
 * Inicialization of stack.
 * @return New stack.
 */
expression_stack *expression_stack_init()
{
    expression_stack *new = malloc(sizeof(expression_stack));
    new->top = NULL;
    return new;
}


/**
 * Put list on top of stack.
 * @param target Pointer to stack.
 * @param data Type of list.
 */
void expression_stack_push(expression_stack *target, symbol_enum data)
{
    expression_list *item = malloc(sizeof(expression_list));
    item->symbol = data;
    target->top = item;
}


/**
 * Destroy list on top of stack.
 * @param target Pointer to stack.
 */
void expression_stack_pop(expression_stack *target)
{
    expression_list *successor = target->top->next;
    free(target->top);
    target->top = successor;
}


/**
 * Return first list which is terminal.
 * @param target Pointer to stack.
 * @return first list which is terminal.
 */
expression_list* stack_top_terminal(expression_stack* target)
{
    for (expression_list* list = target->top; list != NULL; list = list->next)
    {
        if (list->symbol < STOP)
            return list;
    }
    return NULL;
}


/**
 *
 * @param target Pointer to stack.
 * @param symbol
 * @return
 */
bool insert_after_top_terminal(expression_stack* target, symbol_enum symbol)
{
    expression_list* previous = NULL;

    for (expression_list* tempo = target->top; tempo != NULL; tempo = tempo->next)
    {
        if (tempo->symbol < STOP)
        {
            expression_list* new_item = (expression_list*)malloc(sizeof(expression_list));
            new_item->symbol = symbol;

            if (previous != NULL)
            {
                new_item->next = previous->next;
                previous->next = new_item;
            }
            else
            {
                new_item->next = target->top;
                target->top = new_item;
            }
            return true;
        }
        previous = tempo;
    }
    return false;
}


/**
 *
 * @param target Pointer to stack.
 * @return Head of stack.
 */
expression_list* expression_stack_top(expression_stack *target)
{
    return target->top;
}


/**
 * Destroys stack.
 * @param target Pointer to stack.
 */
void expression_stack_destroy(expression_stack *target)
{
    while(!expression_stack_empty(target))
    {
        expression_stack_pop(target);
    }
}


/**
 *
 * @param target Pointer to stack.
 * @return True if stack is empty, otherwise false.
 */
bool expression_stack_empty(expression_stack *target)
{
    if (target->top){
        return false;
    }
    return true;
}
