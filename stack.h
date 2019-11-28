//
// Created by Marek Linner on 29.09.2019.
//

#ifndef SOURCE_STACK_H
#define SOURCE_STACK_H

#include "structs.h"
#include "scanner.h"

Stack initStack();
int top(Stack target_stack);
int pop(Stack *target_stack);
void push(Stack *target_stack, int number);
void printStackContent(Stack target_stack);

TStack TinitStack();
Token Ttop(TStack target_stack);
Token Tpop(TStack *target_stack);
void Tpush(TStack *target_stack, Token token);
void printTStackContent(TStack *target_stack);

int precedence(Token *oper);
void saveToken1(TStack *L);
void saveToken2(TStack *L);
TStack infix_2_postfix();

#endif //SOURCE_STACK_H
