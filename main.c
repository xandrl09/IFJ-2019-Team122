/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "structs.h"
#include "symtable.h"
#include "stack.h"
#include "parser.h"
#include "generator.h"
#include "stack_cd.h"

tDLList *tokenQueue;
CDStack *code_gen_stack;



int main(int argc, char* argv[])	{
    tokenQueue = malloc(sizeof(tDLList));
    DLInitList(tokenQueue);
    if (USING_PROG_ARGS)
        createScanner(argv[1]);			
    init_symtable();
    code_gen_stack = CDinit_stack();
    gen_init();
    parse();

    drop_symtable();
    DLDisposeList(tokenQueue);
    return 0;
}

