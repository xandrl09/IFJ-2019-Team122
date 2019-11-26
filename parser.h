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

    T_token *token;
    T_token *second_token;
    T_token *third_token;

    T_token *func_id;

    char* str;

    bool in_function;			/// Defines if the parser is in function

    bool non_declared_function;	/// Function that has been only defined

    STsymPtr symb; /// sym
} MainData;


int parse();
int expression(MainData* data);

static int main_body(MainData* data);
static int main_func(MainData* data);
static int def_func_params(MainData* data);
static int func_param_x(MainData* data);
static int main_(MainData* data);
static int code(MainData* data);
static int identif(MainData* data);
static int ins(MainData* data);
static int ins_id(MainData* data);
static int call_func_params(MainData* data);
static int call_func_param_x(MainData* data);
static int inner_func(MainData* data);
static int term(MainData* data);