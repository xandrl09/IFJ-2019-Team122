/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#include "scanner.h"
#include "stack_ex.h"
#include <stdlib.h>
#include <stdio.h>

extern tDLList *tokenQueue;

int top(Stack target_stack) {
    return target_stack.arr[target_stack.top];
}

Token Ttop(TStack target_stack)    {
    return *target_stack.arr[target_stack.top];
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

TStack TinitStack() {
    int initialStackSize = 255;
    int top = 0;
    TStack *myStack = malloc(sizeof(TStack));
    for(int i = 0; i < initialStackSize; i++) {
        myStack->arr[i] = NULL;
    }
    myStack->top = top;
    return *myStack;
}

int pop(Stack *target_stack) {
    int res = target_stack->arr[target_stack->top];
    target_stack->arr[target_stack->top--] = -1;
    return res;
}

Token Tpop(TStack *target_stack) {
    Token *res = target_stack->arr[target_stack->top];
    target_stack->arr[target_stack->top--] = NULL;
    return *res;
}

void push(Stack *target_stack, int number) {
    target_stack->arr[++target_stack->top] = number;
}

void Tpush(TStack *target_stack, Token token) {
    target_stack->arr[target_stack->top++] = &token;
    //target_stack->top++;
}

void printStackContent(Stack *target_stack) {
    for(int i = 0; i <= target_stack->top; i++)   {
        if(target_stack->arr[i] == -1)
            return;
        printf("%i: %i\n", i, target_stack->arr[i]);
    }
}

void printTStackContent(TStack *target_stack) {
    for(int i = 0; i <= target_stack->top; i++)   {
        if(target_stack->arr[i] == NULL)
            return;
        printf("%i: %s\n", i, target_stack->arr[i]->value);
    }
}

int precedence(Token *oper) {
    if(oper->value[0] == '^') {
        return 3;
    }
    else if(oper->value[0] == '*' || oper->value[0] == '/') {
        return 2;
    }
    else if(oper->value[0] == '+'  || oper->value[0] == '-') {
        return 1;
    }
    else {
        return 0;
    }
}

void saveToken1(TStack *L) {
    Token *token = malloc(sizeof(Token));
    token->type = specialChar;
    token->value[0] = '(';
    token->value[1] = '0';
    token->rptr = NULL;
    token->lptr = NULL;
    token->positionInLine = -1;
    L->arr[1] = token;
    L->top = 1;
}

void saveToken2(TStack *L) {
    Token *token = malloc(sizeof(Token));
    token->type = specialChar;
    token->value[0] = ')';
    token->value[1] = '0';
    token->rptr = NULL;
    token->lptr = NULL;
    token->positionInLine = -1;
    L->arr[L->top++] = token;

}

TStack infix_2_postfix(tDLList *queue) {
    int i = 0;
    DLFirst(queue);
    tDLList *tmp = queue;
    Token *token;
    TStack lil_stack = TinitStack();
    TStack infix = TinitStack();
    TStack postfix = TinitStack();
    saveToken1(&lil_stack);

    for(int k = 0; k < 255; k++) {
        if(tmp->Act != NULL && tmp->Act->type != EoF && tmp->Act->type != EOL) {
            infix.arr[k] = tmp->Act;
            infix.top++;
            DLSucc(tmp);
        }
        else {
            break;
        }
    }
    saveToken2(&infix);
    Token *item = infix.arr[i];

    if(item->type != EOL) {
        while (item->type != EOL && item->type != EoF) {

            if (item->value[0] == '(') {
                lil_stack.arr[++lil_stack.top] = item;
            }
            else if (item->type == identifier || item->type == integer || item->type == floatingPoint) {
                postfix.arr[postfix.top++] = item;
            }
            else if (isOperator(item->value) == 1) {
                token = lil_stack.arr[lil_stack.top];
                lil_stack.arr[lil_stack.top--] = NULL;
                while (isOperator(token->value) == 1 && precedence(token) >= precedence(item)) {
                    postfix.arr[postfix.top++] = token;
                    token = lil_stack.arr[lil_stack.top];
                    lil_stack.arr[lil_stack.top--] = NULL;
                }
                lil_stack.arr[++lil_stack.top] = token;
                lil_stack.arr[++lil_stack.top] = item;

            }
            else if (item->value[0] == ')') {
                token = lil_stack.arr[lil_stack.top];
                lil_stack.arr[lil_stack.top--] = NULL;
                while (token->value[0] != '(') {
                    postfix.arr[postfix.top++] = token;
                    token = lil_stack.arr[lil_stack.top];
                    lil_stack.arr[lil_stack.top--] = NULL;
                }
            }
            i++;

            item = infix.arr[i];
            if(item == NULL)
                break;
        }
        return postfix;
    }
    else {
        errInter();
        exit(errINTER);
    }
}