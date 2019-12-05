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


/**
 * enum for rows in table
 */
typedef enum
{
    T_REL,		/// 0 r
    T_PLUS,		/// 1 +-
    T_MUL_DI,  /// 2 */ //
    T_LEFT_B,	/// 3 (
    T_RIGHT_B,	/// 4 )
    T_IDS,	    /// 5 i
    T_END		/// 6 $

} index_enum;


/**
 * enum for rules in table
 */
typedef enum
{
    S,    /// < SHIFT
    Q,    /// = EQUAL
    R,    /// > REDUCE
    E     /// # ERROR
} Precedence_table_sign_enum;








