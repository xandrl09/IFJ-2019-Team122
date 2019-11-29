/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#ifndef IFJ_QUEUE
#define IFJ_QUEUE

#include <string.h>
#include "structs.h"


/* prototypy jednotlivých funkcí */
void DLInitList (tDLList *L);
void DLDisposeList (tDLList *L);
void DLInsertLast(tDLList *L, tokenType ttype, char *val, int positionInLine);
void DLFirst (tDLList *L);
void DLLast (tDLList *L);
void DLCopy(tDLList *L, tokenType *ttype, char *val, int *positionInLine);
void DLSucc (tDLList *L);

int appendToken(tDLList *L, Token *token, Token *expression, int index);
void printQueueContents(tDLList *L);
int howManyElementsInQueue(tDLList *L);
char* getTokenTypeStringValue(tokenType type);

#endif
