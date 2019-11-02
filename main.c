#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "structs.h"
#include "symtable.h"
#include "stack.h"
#include "parser.h"

STmaintablePtr globalSymTable;
extern Stack indentationStack;
tDLList *tokenQueue;

void evaluateInput(int argc)	{
    if (argc != 2)	{
        fprintf(stderr, "Progam failed during the evaluateInput func\n");
        	exit(1);
    }
}

int main(int argc, char* argv[])	{
    tokenQueue = malloc(sizeof(tDLList));
    //evaluateInput(argc);
    createScanner(argv[1]);			//argv[1] == path
    DLInitList(tokenQueue);
    //indentationStack = initStack();
    globalSymTable = init_table(10);

    // this is how you get a token from scanner ready to be parsed
//    T_token *token;
//    token = token_init(token);
//    getParserToken(token);

    parse();

    table_delete(&globalSymTable);
    DLDisposeList(tokenQueue);
    return 0;
}

