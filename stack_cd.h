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

CDStack * CDinit_stack();
char* CDpop(CDStack *stack);
void CDpush(CDStack *stack, char* line);
char* CDtop(CDStack stack);
char* copy_string(char *str);
void CDStack_destroy(CDStack *stack);
void CDStack_free_elems(CDStack *stack);



#endif //PROJEKT_STACK_CD_H
