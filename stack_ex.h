

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "expresions.h"
#include "symtable.h"
#include "main.h"

typedef struct list_ex{
    symbol_enum symbol;
    struct list_ex *next;
}list_exp;


typedef struct stack_ex{
    list_exp *head;
}stack_exp;


stack_exp  *ex_stack_init();
bool ex_stack_empty(stack_exp *target);


void ex_stack_pop(stack_exp *target);

int ex_stack_push(stack_exp *target,  symbol_enum data);

void repeted_pop(stack_exp * target, int times);

list_exp* stack_top_terminal(stack_exp* target);

bool insert_after_top_terminal(stack_exp* target,  symbol_enum symbol);

list_exp* ex_stack_top(stack_exp* target);

void ex_stack_destroy(stack_exp *target);




