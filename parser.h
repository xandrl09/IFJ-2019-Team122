/**
 * Subject: IFJ & IAL
 *
 * Project: Implementation of project IFJ18.
 *
 * @brief Parser.
 *
 * @author Ondřej Andrla <xandrl09@stud.fit.vutbr.cz>
 */

#pragma once

#include "scanner.h"
#include "err.h"
#include "symtable.h"
//#include "func.h"
#include "expresions.h"

// hlavicka pro syntakticky analyzator


/**
 * @struct Parser's internal data representation.
 */
typedef struct
{

    STmaintablePtr table; /// CHECK

    T_token token;
    T_token second_token;
    T_token third_token;

    T_token func_id;

    char* str;

    bool in_function;			/// Defines if the parser is in function

    bool non_declared_function;	/// Function that has been only defined

    STsymPtr symb; /// sym
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