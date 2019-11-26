//
// Created by Marek on 11/23/2019.
//

#ifndef PROJEKT_STACK_CD_H
#define PROJEKT_STACK_CD_H

#include "structs.h"
#include <stdlib.h>

CDStack CDinit_stack();
char* CDpop(CDStack *stack);
void CDpush(CDStack *stack, char* line);
char* CDtop(CDStack stack);


#endif //PROJEKT_STACK_CD_H
