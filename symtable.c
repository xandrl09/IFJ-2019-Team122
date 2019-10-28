/**
 * Implementace překladače imperativního jazyka IFJ18
 *
 * xscevi00   Ščevik Ľuboš
 * xlinne00   Linner Marek
 * xstoja06   Stojan Martin
 * xbakal01   Bakaľárová Alica
 *
 */

#include "symtable.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define TRUE 1
#define FALSE 0

/*
*Function allocates memory for the variable structure and defines it with the given values.
*/
STdivPtr createVar(union STvalue value, dataType type){//function for further calling
	STvarPtr varptr = (STvarPtr)malloc(sizeof(struct STvar));
	if (varptr == NULL) {
		fprintf(stderr, "ERROR / space for var not allocated");
		errInter();
	}
	else {
		varptr->value = value;
		varptr->type = type;
		varptr->gen_used=FALSE;
	}

	STdivPtr divptr = (STdivPtr)malloc(sizeof(struct STdiv));
	if (divptr == NULL) {
		fprintf(stderr, "ERROR / space for div-var not allocated");
		free(varptr);
		errInter();
	}
	divptr->variable = varptr;
	return divptr;
}

/*
*Function allocates memory for the function structure and defines it with the given values.
*/
STdivPtr createFoo(dataType returntype, unsigned int attrcount, tDLList *paramQueue, bool declared, bool defined){//function for further calling
	//treba spravit osetrenie parametrov???
	STfunctionPtr fooptr = (STfunctionPtr)malloc(sizeof(struct STfunction));
	if (fooptr == NULL) {
		fprintf(stderr, "ERROR / space for function not allocated");
		exit(-1);
	}
	fooptr->paramQueue =paramQueue;
	fooptr->returnvalue = returntype;
	fooptr->numberofattr = attrcount;
	fooptr->declared = declared;
	fooptr->defined = defined;
	fooptr->cycle_aux=0;
	fooptr->condition_aux=0;
	fooptr->local_table=NULL;

	STdivPtr divptr = (STdivPtr)malloc(sizeof(struct STdiv));
	if (divptr == NULL) {
		fprintf(stderr, "ERROR / space for div-function not allocated");
		free(fooptr);
		errInter();
	}
	divptr->function = fooptr;
	return divptr;
}

/*
*Function allocates memory for the symbol and defines it with the given values.
*/
STsymPtr createSym(symType type, STdivPtr divptr, char *identifier){//function for further calling
	STsymPtr symptr = (STsymPtr)malloc(sizeof(struct STsym));
	if (symptr == NULL) {
		fprintf(stderr, "ERROR / space for symbol not allocated");
		errInter();
	}
	else{
		(*symptr).identifier = identifier;
		symptr->type = type;
		symptr->div = divptr;
		symptr->next=NULL;
		symptr->before=NULL;
	}
	return symptr;
}

/*
* Function allocates memory for the table of symbols header and for the hasharray of pointers to symbols.
* Then it initializes pointers to NULL.
*/
STmaintablePtr init_table(unsigned int size){//function for further calling

	STmaintablePtr tableptr=(STmaintablePtr)malloc(sizeof(struct STmaintable));

	if (tableptr == NULL) {
		fprintf(stderr, "ERROR / space for table not allocated");
		errInter();
	}

	tableptr->sizeoftable=size;
	tableptr->symbolcount=0;

	if ( ! (tableptr->hash_arr = (STsymPtr *)malloc(size * sizeof(STsymPtr)))) {
		free(tableptr);
		fprintf(stderr, "ERROR / space for array of pointers to symbols not allocated");
		errInter();
	}

	// initializations of hashelements in the array
	for (unsigned int i = 0; i < size; i++) {
		tableptr->hash_arr[i]=(STsymPtr)NULL;
	}

	return tableptr;
}

/*
* Function checks if there is already a symbol at the given index of hasharray.
* If no, it inserts the given symbol into the first index.
* If yes, it inserts the given symbol into the first index of the array of symbols
* with the same hash and set the next and before pointers.
* If the table is half full, it makes a new table, rehashes all the symbols and free the old table header and hasharray.
*/
void table_insert(STmaintablePtr *tableptr, STsymPtr *symptr) {//function for further calling

	if(sym_search_ptr(tableptr, (*symptr)->identifier)) //if the symbol with the same identifier already exists in the table
		symbol_remove(tableptr, (*symptr)->identifier); //it removes it
	//^^^^^NOT TESTED^^^^^

	//spravit kontrolu parametrov?
	unsigned int hash_number=get_hash((*symptr)->identifier, (*tableptr)->sizeoftable);
	((*tableptr)->symbolcount)++;
	if ((*tableptr)->symbolcount > (*tableptr)->sizeoftable)
		errInter();

	if ((*tableptr)->hash_arr[hash_number] == NULL) {//inserts as the first element into empty array
		(*tableptr)->hash_arr[hash_number]=*symptr;
	}
	else{////inserts as the first element into non-empty array
		(*symptr)->next=(*tableptr)->hash_arr[hash_number];
		(*tableptr)->hash_arr[hash_number]=*symptr;
		(*symptr)->next->before=*symptr;
	}

	unsigned int number=(*tableptr)->sizeoftable;
//	number/=2;

/*
	if(!((*tableptr)->symbolcount < number)) {
		unsigned int doublesize = (*tableptr)->sizeoftable; doublesize *= 2;
		STmaintablePtr secondtable = init_table(doublesize);
		STsymPtr symptr2, sym_next;

		for (unsigned int i = 0; i < (*tableptr)->sizeoftable; i++) {
			symptr2 = (*tableptr)->hash_arr[i];
			while(symptr2!=NULL){
				sym_next=symptr2->next;
				table_insert_res(&secondtable, &symptr2);
				symptr2=sym_next;
			}
		}
		free((*tableptr)->hash_arr);
		free(*tableptr);
		*tableptr=secondtable;
	}
*/
}

//Table insert for table_resize function
void table_insert_res(STmaintablePtr *tableptr, STsymPtr *symptr) {

	unsigned int hash_number=get_hash((*symptr)->identifier, (*tableptr)->sizeoftable);
	(*tableptr)->symbolcount++;

	if ((*tableptr)->hash_arr[hash_number] == NULL) {//inserts as the first element into empty array
		(*tableptr)->hash_arr[hash_number]=*symptr;
		(*symptr)->before=NULL;
		(*symptr)->next=NULL;
	}
	else{//inserts as the first element into non-empty array
		(*symptr)->next=(*tableptr)->hash_arr[hash_number];
		(*tableptr)->hash_arr[hash_number]=*symptr;
		(*symptr)->next->before=*symptr;
		(*symptr)->before=NULL;
	}
}

/*
*Function exchanges symbol with his left neighbour in the list. (We are sure that symbol has at least one left neighbour.)
*/
void sym_exchg(STsymPtr *symptr){//right neighbour
	STsymPtr sym_next, sym_before;

	if((*symptr)->before->before!=NULL)//if the symbol is at least third
		(*symptr)->before->before->next=(*symptr);
	if((*symptr)->next!=NULL)//if symbol is not the last
		(*symptr)->next->before=(*symptr)->before;

	sym_next=(*symptr)->next;
	sym_before=(*symptr)->before->before;

	(*symptr)->next=(*symptr)->before;//some incomprehensible magic
	(*symptr)->before->next=sym_next;
	(*symptr)->before->before=*symptr;
	(*symptr)->before=sym_before;
}

/*
* Function searches for the element with the given identifier, returns true if finds it, false if not.
* Works only for searching with identifier.
*/
bool sym_search(STmaintablePtr *tableptr, char *identifier) {//function for further calling

	if (identifier == 0)
		return FALSE;

	if(*tableptr == NULL || identifier == NULL) {
		fprintf(stderr, "Bad parameters in foo sym_search");
		errInter();
	}

	unsigned int hash_number=get_hash(identifier, (*tableptr)->sizeoftable);
	STsymPtr symptr=(*tableptr)->hash_arr[hash_number];

	while(symptr!=NULL){//if index is not empty
		if(strcmp(symptr->identifier, identifier) == 0){//if identifiers are equal
			if(symptr->before!=NULL){//if searched symbol is not first
				sym_exchg(&symptr);//moving searched symbol one item to the left
				if(symptr->before==NULL)//if the symbol is first after exchange
					(*tableptr)->hash_arr[hash_number]=symptr;//saves pointer to the symbol to the hasharray
			}
			return TRUE;
		}
		symptr=symptr->next;
	}
	return FALSE;
}

/*
* Function searches for the element with the given identifier, returns pointer to the symbol if finds it, NULL if not.
*/
STsymPtr sym_search_ptr(STmaintablePtr *tableptr, char *identifier) {//function for further calling

	if(*tableptr == NULL || identifier == NULL) {
		fprintf(stderr, "Bad parameters in foo sym_search_ptr");
		errInter();
	}
	unsigned int hash_number=get_hash(identifier, (*tableptr)->sizeoftable);
	STsymPtr symptr=(*tableptr)->hash_arr[hash_number];

	while(symptr!=NULL){
		if(strcmp(symptr->identifier, identifier) == 0){
			if(symptr->before!=NULL){
				sym_exchg(&symptr);//moving searched symbol one item to the left
				if(symptr->before==NULL)
					(*tableptr)->hash_arr[hash_number]=symptr;
			}
			return symptr;
		}
		symptr=symptr->next;
	}
	return NULL;
}

/*
*Function frees all the parts of symbol structure; if the given symbol doesnt exist it does nothing.
*/
void symbol_free(STsymPtr *symptr){
	if(symptr!=NULL){
		if ((*symptr)->type == variable){
			free((*symptr)->div->variable);
			free((*symptr)->div);
		}
		else {
			free((*symptr)->div->function);
			free((*symptr)->div);
		}
		free(*symptr);
		*symptr=NULL;
	}
}

/*
*Function removes a symbol with the given identifier and type. If it doesnt find the symbol, it does nothing.
*/
void symbol_remove(STmaintablePtr *tableptr, char *identifier){//function for further calling

	unsigned int hash_number = get_hash(identifier, (*tableptr)->sizeoftable);
	STsymPtr symptr = sym_search_ptr(tableptr, identifier);

	if(symptr!=NULL)
		(*tableptr)->symbolcount--;

	if(symptr->before == NULL){//if symbol is first
		(*tableptr)->hash_arr[hash_number]=symptr->next;
		if(symptr->next!=NULL)//if symbol is not last
			symptr->next->before=NULL;
		symbol_free(&symptr);
		return;
	}
	else{//if symbol is not first
		symptr->before->next=symptr->next;
		if(symptr->next!=NULL)//if symbol is not last
			symptr->next->before=symptr->before;
		symbol_free(&symptr);
		return;
	}
}

/*
*Function frees table and symbols (at the end).
*/
void table_delete(STmaintablePtr *tableptr) {//function for further calling
	if (tableptr != NULL) {
		STsymPtr symptr, symptr_next;
		for (unsigned int i = 0; i < (*tableptr)->sizeoftable; i++) {
			symptr = (*tableptr)->hash_arr[i];
			while(symptr!=NULL){
				symptr_next=symptr->next;
				symbol_free(&symptr);
				symptr=symptr_next;
			}
		}

		free((*tableptr)->hash_arr);
		(*tableptr)->hash_arr=NULL;
		free(*tableptr);
		*tableptr=NULL;
	}
}

//Hash function stolen from http://www.seg.rmit.edu.au/code/zwh-ipl/bitwisehash.c
unsigned int get_hash(char *word, int tsize) {
	//int tsize = TSIZE;
	char c;
	unsigned int h = SEED;

	for( ; ( c=*word )!='\0' ; word++ )
	{
		h ^= ( (h << 5) + c + (h >> 2) );
	}
	return((unsigned int)((h&0x7fffffff) % tsize));
}

void printtable(STmaintablePtr *tableptr){
	printf("-----------------------------\n");
	for (unsigned int i = 0; i < (*tableptr)->sizeoftable; i++) {
		STsymPtr symptr = (*tableptr)->hash_arr[i];
		printf("Index %u: ", i);
		while(symptr!=NULL){
			if (symptr->type == function) {
				printf("\nFunkce;\tID: %s\tAdresa: %p, Return type: %i, NumOfParams: %i", symptr->identifier, symptr, symptr->div->function->returnvalue, symptr->div->function->numberofattr);

				if (symptr->div->function->paramQueue->First!= NULL)   {
					symptr->div->function->paramQueue->Act=symptr->div->function->paramQueue->First;
					//for (int i = 1; i <= symptr->div->function->numberofattr; i++)  {
					for (int i = 1; i <= howManyElementsInQueue(symptr->div->function->paramQueue); i++)  {
						printf("\nJeji %i. parametr = |%s|", i, symptr->div->function->paramQueue->Act->value);
						symptr->div->function->paramQueue->Act = symptr->div->function->paramQueue->Act->rptr;
					}
				}
			}
			else
				printf("\nPromenna = { ID= |%s|, Adresa = |%p|, Hodnota = |%i|", symptr->identifier, symptr, symptr->div->variable->value.i);

			symptr=symptr->next;
		}
		printf("\n");
	}
}
