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
 * @enum Symbols used for precednece analysis.
 */
typedef enum symbol_enum
{
    PLUS,			/// +
    MINUS,			/// -
    MUL_,			/// *
    DIV_,			/// /
    DIV_WH,         /// //
    EQ_,			/// =
    NEQ,			/// <>
    LEQ,			/// <=
    LESS,			/// <
    MEQ,			/// >=
    MORE,			/// >
    LEFT_BRACKET_,	/// (
    RIGHT_BRACKET_,	/// )
    ID,		/// ID
    INT_,		/// int
    FLOAT_,	/// double
    STRING_,			/// string
    DOLLAR,			/// $
    STOP,			/// stop symbol used when reducing
    NON_TERM		/// non-terminal
} symbol_enum;


