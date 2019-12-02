/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#include "err.h"
#include <stdio.h>
#include <stdlib.h>

void errLex(){
    printf("Error occurred during lexical analysis!\n");
    //exit(errLEX);
}

void errSyn(){
    fprintf(stderr, "Error occurred during syntactic analysis!\n");
    exit(errSYN);
}

void errSemDef(){
    fprintf(stderr, "Error occurred during semantic analysis!\n");
    exit(errSEMDEF);
}

void errSemType(){
    fprintf(stderr, "Error occurred during semantic analysis!\n");
    exit(errSEMTYPE);
}

void errSemOth(){
    fprintf(stderr, "Error occurred during semantic analysis!\n");
    exit(errSEMOTH);
}

void errZero(){
    fprintf(stderr, "Error occurred during dividing by zero!\n");
    exit(errZERO);
}

void errInter(){
    fprintf(stderr, "Error: Internal translator mistake!\n");
    exit(errINTER);
}

void errSemWrongParam() {
    fprintf(stderr, "Error: Wrong number of arguments!\n");
    exit(errSEMWRONGPARAM);
}
