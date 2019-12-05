/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#pragma once

/**
 * @enum This enum is for expresions.c.
 * It
 */
typedef enum symbol_enum
{
    PLUS_,			/// +
    MINUS_,			/// -
    MUL_,			/// *
    DIV_,			/// /
    DIV_WH_,         /// //
    EQ_,			/// =
    N_EQ,			/// <>
    L_EQ,			/// <=
    LESS,			/// <
    M_EQ,			/// >=
    MORE,			/// >
    LEFT_BRACK_,	/// (
    RIGHT_BRACK_,	/// )
    ID,		/// ID
    INT_,		/// int
    FLOAT_,	/// double
    STRING_,			/// string
    DOLLAR,			/// $
    STOP,			/// stop symbol used when reducing
    NO_TERMINAL		/// non-terminal
} symbol_enum;


