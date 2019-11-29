/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#ifndef PROJEKT_STACK_CD_H
#define PROJEKT_STACK_CD_H

#include "structs.h"
#include <stdlib.h>

CDStack CDinit_stack(CDStack *code_gen_stack);
char* CDpop(CDStack *stack);
void CDpush(CDStack *stack, char* line);
char* CDtop(CDStack stack);


#endif //PROJEKT_STACK_CD_H
