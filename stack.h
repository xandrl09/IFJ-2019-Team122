//
// Created by Marek Linner on 29.09.2019.
//

#ifndef SOURCE_STACK_H
#define SOURCE_STACK_H

#include "structs.h"

Stack initStack();
int top(Stack stack);
int pop(Stack *stack);
void push(Stack *stack, int number);
void printStackContent(Stack stack);

TStack TinitStack();
Token Ttop(TStack stack);
Token Tpop(TStack *stack);
void Tpush(TStack *stack, Token token);
void printTStackContent(TStack stack);

#endif //SOURCE_STACK_H
