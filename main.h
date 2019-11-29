/**
 * Author: Ondřej Andrla
 * datum: 2019
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
    LEFT_BRACK_,	/// (
    RIGHT_BRACK_,	/// )
    ID,		/// ID
    INT_,		/// int
    FLOAT_,	/// double
    STRING_,			/// string
    DOLLAR,			/// $
    STOP,			/// stop symbol used when reducing
    NON_TERM		/// non-terminal
} symbol_enum;


