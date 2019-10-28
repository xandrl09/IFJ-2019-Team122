//
// Created by Marek Linner on 29.09.2019.
//

#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

int top(Stack stack) {
    return stack.arr[stack.top];
}

Token Ttop(TStack stack)    {
    return *stack.arr[stack.top];
}

Stack initStack() {
    int stackSize = 255;
    int content[stackSize];
    for(int i = 0; i < stackSize; i++)
        content[i] = -1;
    int top = 0;
    Stack *myStack = malloc(sizeof(content) + sizeof(top));
    for(int i = 0; i < stackSize; i++) {
        myStack->arr[i] = -1;
    }
    myStack->top = top;
    myStack->arr[0] = 0;
    return *myStack;
}

TStack initTStack() {
    int initialStackSize = 255;
    int top = 0;
    TStack *myStack = malloc(sizeof(Token)* initialStackSize + sizeof(top));
    for(int i = 0; i < initialStackSize; i++) {
        myStack->arr[i] = NULL;
    }
    myStack->top = top;
    return *myStack;
}

int pop(Stack *stack) {
    int res = stack->arr[stack->top];
    stack->arr[stack->top--] = -1;
    return res;
}

Token Tpop(TStack *stack) {
    Token *res = stack->arr[stack->top];
    stack->arr[stack->top--] = NULL;
    return *res;
}

void push(Stack *stack, int number) {
    stack->arr[++stack->top] = number;
}

void Tpush(TStack *stack, Token token) {
    stack->arr[stack->top] = &token;
    stack->top++;
}

void printStackContent(Stack stack) {
    for(int i = 0; i <= stack.top; i++)   {
        if(stack.arr[i] == -1)
            return;
        printf("%i: %i\n", i, stack.arr[i]);
    }
}

void printTStackContent(TStack stack) {
    for(int i = 0; i <= stack.top; i++)   {
        if(stack.arr[i] == NULL)
            return;
        printf("%i: %s\n", i, stack.arr[i]->value);
    }
}