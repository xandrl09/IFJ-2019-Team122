/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00   Linner Marek
 * xstoja06   Stojan Martin
 *
 */


#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

#define TESTMODE 0


void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
**/

    //seznam inicializujeme nastavenim vsech jeho pointeru na NULL
    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free.
**/
    if(L == NULL)	{
        printf("kokotko");
    }
    if (L->First != NULL) { //pokud seznam neni prazdny
        L->Last = NULL;
        struct Token *elemToBeDeleted = L->First; //pointer na mazany prvek
        L->Act = L->First;  //posunuti aktivity na zacatek seznamu

        //s kazdou iteraci se aktivni pointer posunuje o 1 doprava a prvek
        //pred aktivnim se uvolnuje, dokud nedorazi na konec
        while (L->Act != NULL)  {
            L->Act = L->Act->rptr;
            free(elemToBeDeleted);
            elemToBeDeleted = L->Act;
        }
    }
    //nastaveni listu *L do puvodniho stavu
    L->First = NULL;
    L->Last = NULL;
    L->Act = NULL;
}


void DLInsertLast(tDLList *L, tokenType ttype, char *val, int positionInLine) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    //alokace pameti pro novy prvek seznamu
    struct Token *elemPtr = (struct Token*) malloc(sizeof(struct Token));
    if (elemPtr == NULL)    {   //pokud doslo k selhani (nedostatek pameti)
        DLError();
        return;
    }
    else    {
        elemPtr->rptr = NULL;   //prvek na naslednika je nastaven na NULL
        elemPtr->lptr = L->Last;//ukazatel na predchudce ted ukazuje spravne
        elemPtr->type = ttype;
        elemPtr->positionInLine = positionInLine;

        char* differentBuffer = malloc(sizeof(val));
        strcpy(differentBuffer, val);
        elemPtr->value = differentBuffer;

        if (L->Last != NULL)        {   //jestlize seznam neni prazdny
            L->Last->rptr = elemPtr;    //pripojeni prvku na konec seznamu
        }
        else    {       //v prazdnem seznamu je prvak zaroven prvni i posledni
            L->First = elemPtr;
        }
        L->Last = elemPtr;  //uprava ukazatele listu
    }
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
**/

    L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
**/

    L->Act = L->Last;
}

void DLCopy(tDLList *L, tokenType *ttype, char *val, int *positionInLine) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

    if (L->Act == NULL) //pokud seznam neni aktivni
        DLError();
    else    {
        char *val2 = L->Act->value;
        strcpy(val, val2);
        *ttype = L->Act->type;
        *positionInLine = L->Act->positionInLine;
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
**/

    if (L->Act!=NULL)   {
        L->Act = L->Act->rptr;
    }
}

void printQueueContents(tDLList *q) {
    FILE *out;
    if(TESTMODE)
        out = fopen("out.txt", "a");

    Token *tmpToken;
    tmpToken = q->Act;
    q->Act = q->First;
    int i = 0;
    while(q->Act != NULL)   {
        if(TESTMODE)
            fprintf(out, "%s(%i), ", getTokenTypeStringValue(q->Act->type), q->Act->type);
        printf("%i. token { position=%i, value=%s, type=%s, n. %i }\n",
               i, q->Act->positionInLine, q->Act->value, getTokenTypeStringValue(q->Act->type), q->Act->type);

        DLSucc(q);
        i++;
    }
    q->Act = tmpToken;
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    if(TESTMODE)
        fclose(out);
}

char* getTokenTypeStringValue(tokenType type) {
    switch (type)   {
        case error: return "error";
        case keyword: return "keyword";
        case identifier: return "identifier";
        case integer: return "integer";
        case floatingPoint: return "float";
        case Operator: return "operator";
        case string: return "string";
        case specialChar: return "specialChar";
        case EOL: return "EOL";
        case INDENT: return "INDENT";
        case DEDENT: return "DEDENT";
        case docString: return "docString";
        case builtInFunc: return "builtInFunc";
        case EoF: return "EOF";
    }
    return "";
}

int howManyElementsInQueue(tDLList *L) {
    Token *tmpToken;
    tmpToken = L->Act;
    L->Act = L->First;
    int count = 0;
    while (L->Act != NULL)  {
        count++;
        DLSucc(L);
    }
    L->Act = tmpToken;
    return count;
}