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
#include "dynid.h"
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

    dyn_id* str;

    bool in_function;			/// Defines if the parser is in function

    bool non_declared_function;	/// Function that has been only defined

    STsymPtr symb; /// sym
} MainData;


int parse();
int expression(MainData* data);