/**
 * Implementace překladače imperativního jazyka IFJ18
 *
 * xscevi00   Ščevik Ľuboš
 * xlinne00   Linner Marek
 * xstoja06   Stojan Martin
 * xbakal01   Bakaľárová Alica
 *
 */

#ifndef _IFJ_SYMTAB_H
#define _IFJ_SYMTAB_H
#include <stdbool.h>
#include <stdio.h>
#include "err.h"
#include "queue.h"

//all variables behave as local, it is allowed to have just one symbol with certain identifier(is it overwritten or error?)

//initial table size doesnt have to be prime number
#define SEED 1159241

typedef struct STmaintable *STmaintablePtr;

typedef enum {
	i, f, s, voidtype //0,1,2,3
} dataType;

typedef enum {
	variable, function //0,1
} symType;

typedef union STvalue{
	int i;
	float f;
	char *s;
} *STvaluePtr; //types of variable from assignment

typedef struct STvar {
	union STvalue value; //value of variable
	dataType type; //datatype of variable
	bool gen_used; //if its defined in ifjcode
} *STvarPtr;

typedef struct STfunction {
	tDLList *paramQueue;
	dataType returnvalue;
	unsigned int numberofattr;//number of attributes entering the function
	bool declared;//???
	bool defined;
	unsigned int cycle_aux;
	unsigned int condition_aux;
	STmaintablePtr local_table;
} *STfunctionPtr;

typedef struct STdiv {
	STvarPtr variable; //division into functions and variables
	STfunctionPtr function;
} *STdivPtr;

typedef struct STsym {
	char *identifier;
	symType type;//flag
	STdivPtr div;//division betw var and foo, has another information
	struct STsym *next;//pointer to following symbol with the same hash number
	struct STsym *before;
} *STsymPtr;

typedef struct STmaintable { //header of hash table
	unsigned int sizeoftable;  //how many hash indices are in the table
	unsigned int symbolcount;//number of symbols in the table
	STsymPtr *hash_arr; //array of pointers to the symbols
} *STmaintablePtr; //actual table od symbols

//function declarations
STdivPtr createVar(union STvalue value, dataType type);
STdivPtr createFoo(dataType returntype, unsigned int attrcount, tDLList *paramQueue, bool declared, bool defined);
STsymPtr createSym(symType type, STdivPtr divptr, char *identifier);
STmaintablePtr init_table(unsigned int size);

void table_insert(STmaintablePtr *tableptr, STsymPtr *symptr);
void table_insert_res(STmaintablePtr *tableptr, STsymPtr *symptr);
void sym_exchg(STsymPtr *symptr);
bool sym_search(STmaintablePtr *tableptr, char *identifier);
STsymPtr sym_search_ptr(STmaintablePtr *tableptr, char *identifier);
void symbol_free(STsymPtr *symptr);
void symbol_remove(STmaintablePtr *tableptr, char *identifier);
void table_delete(STmaintablePtr *tableptr);
unsigned int get_hash(char *word, int tsize);

void printtable(STmaintablePtr *tableptr);


#endif
