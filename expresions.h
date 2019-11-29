/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#pragma once

#include "parser.h"
#include "stack_ex.h"
#include "main.h"
#include "err.h"
#include "scanner.h"



typedef enum
{
    I_REL_OP,			/// 0 r
    I_PLUS_MINUS,		/// 1 +-
    I_MUL_DIV,		    /// 2 */ //
    I_LEFT_BRACKET,		/// 3 (
    I_RIGHT_BRACKET,	/// 4 )
    I_DATA,	            /// 5 i
    I_DOLLAR			/// 6 $

} index_enum;


typedef enum
{
    S,    /// < SHIFT
    Q,    /// = EQUAL
    R,    /// > REDUCE
    E     /// # ERROR
} Precedence_table_sign_enum;








