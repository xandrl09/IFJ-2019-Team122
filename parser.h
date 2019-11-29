/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#pragma once


#include "structs.h"
//#include "stack.h"
#include "scanner.h"
#include "err.h"
#include "symtable.h"
#include "expresions.h"

// hlavicka pro syntakticky analyzator


/**
 * @struct Parser's internal data representation.
 */
typedef struct
{

    symtable *table; /// CHECK

    T_token token;
    T_token *ptoken;
    T_token second_token;
    T_token third_token;

    T_token func_id;

    char* str;

    char *function_name;


} MainData;


int parse();
int expression(MainData* data);

int main_body(MainData* data);
int main_func(MainData* data);
int def_func_params(MainData* data);
int func_param_x(MainData* data);
int main_(MainData* data);
int code(MainData* data);
int identif(MainData* data);
int ins(MainData* data);
int ins_id(MainData* data);
int call_func_params(MainData* data);
int call_func_param_x(MainData* data);
int inner_func(MainData* data);
int term(MainData* data);