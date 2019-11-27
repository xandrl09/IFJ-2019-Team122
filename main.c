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

//STmaintablePtr globalSymTable;
tDLList *tokenQueue;

void evaluateInput(int param)	{
    if (param != 2)	{
        fprintf(stderr, "Progam failed during the evaluateInput func\n");
        	exit(1);
    }
}

int main(int argc, char* argv[])	{
    tokenQueue = malloc(sizeof(tDLList));
    //evaluateInput(argc);
    createScanner(argv[1]);			//argv[1] == path
    DLInitList(tokenQueue);
    //globalSymTable = init_symtable(10);

//    gen_init();1
//    getLineOfTokens(tokenQueue);  // calling scanner
//    gen_code_from_line(def_line); // calling generator


    //table_delete(&globalSymTable);
    DLDisposeList(tokenQueue);
    return 0;
}

