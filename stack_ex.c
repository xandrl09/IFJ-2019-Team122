

#include "stack_ex.h"


stack_exp *ex_stack_init(){
    stack_exp *new = malloc(sizeof(stack_exp));
    if(new){
        new->head = NULL;
        return new;
    }

    return NULL;
}


list_exp* ex_stack_top(stack_exp* target)
{
    for (list_exp* tmp = target->head; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->symbol < STOP)
            return tmp;
    }

    return NULL;
}

int ex_stack_push(stack_exp *target, symbol_enum data){

    if(target) {
        list_exp *item = malloc(sizeof(list_exp));
        if (item) {
            item->symbol = data;
            if (target->head){
                item->next = target->head;
                target->head = item;

            }
            else{
                item->next = NULL;
                target->head = item;


            }
            return 0;
        }
        else {

            return 1;

        }
    }
    return 1;
}


void ex_stack_pop(stack_exp *target){
    if(target){
        if(target->head){

            list_exp *succ = target->head->next;
            free(target->head);
            target->head = succ;

        }

    }


}


void repeted_pop(stack_exp* target, int times)
{
    for (int i = 0; i < times; i++)
    {
        ex_stack_pop(target);
    }
}


list_exp* stack_top_terminal(stack_exp* target)
{
    for (list_exp* tmp = target->head; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->symbol < STOP)
            return tmp;
    }

    return NULL;
}



bool insert_after_top_terminal(stack_exp* target, symbol_enum symbol)
{
    list_exp* prev = NULL;

    for (list_exp* tmp = target->head; tmp != NULL; tmp = tmp->next)
    {
        if (tmp->symbol < STOP)
        {
            list_exp* new_item = (list_exp*)malloc(sizeof(list_exp));

            if (new_item == NULL)
                return false;

            new_item->symbol = symbol;


            if (prev == NULL)
            {
                new_item->next = target->head;
                target->head = new_item;
            }
            else
            {
                new_item->next = prev->next;
                prev->next = new_item;
            }

            return true;
        }

        prev = tmp;
    }

    return false;
}




void ex_stack_destroy(stack_exp *target){
    if (target){
        list_exp *tmp = target->head;
        if(tmp){
            list_exp *todelete = NULL;

            while(tmp->next!= NULL){
                todelete = tmp;
                tmp = tmp->next;
                free(todelete);
            }
            free(todelete);


        }
            free(target);

    }
}

bool ex_stack_empty(stack_exp *target){
    if(target){
        if (target->head){
            return false;
        }
    }

    return true;
}
