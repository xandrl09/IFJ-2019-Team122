/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */


#include <stdio.h>
#include "parser.h"
#include "generator.h"

#define GET_TOKEN()\
getParserToken(&data->token)

#define CHECK_TYPE(_type)											\
if (data->token.type != (_type)) errSyn()


/// ==================================================================
/// jednotlive funkce odpovidajici jednotlivym nonterminalum gramatiky
/// ==================================================================


/**
 * Function process main body of program.
 * There can be definitions of functions, code and end of file.
 * @param data
 * @return
 */
 int main_body(MainData* data)
{
    GET_TOKEN();
    switch(data->token.type)
    {
        case T_DEF:
            gen_code_from_line(def_line);
            ///Pravidlo 1: def ID ( <func_params> ) :EOL INDENT <main_func> DEDENT <main_body>

            GET_TOKEN();
            CHECK_TYPE(T_ID);

            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);

            GET_TOKEN();
            def_func_params(data);


            GET_TOKEN();
            CHECK_TYPE(T_COLON);
            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            GET_TOKEN();
            CHECK_TYPE(T_INDENT);

            GET_TOKEN();
            main_func(data);
            CHECK_TYPE(T_EOL);
            GET_TOKEN();
            while(data->token.type != T_DEDENT)
            {
                main_func(data);
                CHECK_TYPE(T_EOL);
                GET_TOKEN();
            }
            gen_code_from_line(return_line);

            gen_code_from_line(dedent);
            return main_body(data);

        case T_IF:
        case T_WHILE:
        case T_ID:
        case T_INPUTS:
        case T_INPUTI:
        case T_INPUTF:
        case T_PRINT:
        case T_LENGTH:
        case T_SUBSTR:
        case T_ORD:
        case T_CHR:
        case T_INT:
        case T_FLOAT:
        case T_STRING:
        case T_EOL:
           /// Pravidlo 2 <main_body> -> <code> <main_body>
            code(data);
            return main_body(data);

        case T_EOF:
            /// Pravidlo 3 <main_body> -> e
            gen_code_from_line(end_of_feed);
            return SYNTAX_OK;

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Function process code in body of function, which is currently defined.
 * @param data
 * @return
 */
 int main_func(MainData* data)
{
    switch (data->token.type)
    {
        case T_IF:
        case T_WHILE:
        case T_ID:
        case T_INPUTS:
        case T_INPUTI:
        case T_INPUTF:
        case T_PRINT:
        case T_LENGTH:
        case T_SUBSTR:
        case T_ORD:
        case T_CHR:
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            /// pravidlo 38: <main> -> <code> <main_func>
            code(data);
            return main_func(data);

        case T_EOL:
            ///pravidlo 36: <main> -> EOL<main>
            return SYNTAX_OK;

        case T_PASS:
            return code(data);

        case T_RETURN:
            /// pravidlo 39: return <expr> <main_func>
            expression(data);
            return main_func(data);

        case T_DEDENT:
            /// pravidlo 40: <main_func> -> e
            return SYNTAX_OK;

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Function process first param of function, which is currently defined.
 * @param data
 * @return
 */
 int def_func_params(MainData* data)
{
    switch (data->token.type)
    {
        case T_ID:
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            /// pravidlo 4: <def_func_params> -> "ID" <func_param_x>
            GET_TOKEN();
            func_param_x(data);
            return SYNTAX_OK;

        case T_RBRACK:
            /// pravidlo 5: <def_func_params> -> e
            return SYNTAX_OK;
        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Function process params of function, which is currently defined.
 * @param data
 * @return
 */
 int func_param_x(MainData* data)
{
    switch (data->token.type)
    {
        case T_COMMA:
            /// pravidlo 6: <func_param_x> -> "," "ID" <func_param_x>

            GET_TOKEN();
            if (data->token.type == T_ID || data->token.type == T_STRING || data->token.type == T_INT || data->token.type == T_FLOAT )
            {
            }
            else
            {
                errSyn();
                return SYNTAX_ERROR;
            }

            GET_TOKEN();
            func_param_x(data);

        case T_RBRACK:
             /// pravidlo 7: <func_param_x> -> e
             return SYNTAX_OK;
        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Function process code in main body of program.
 * @param data
 * @return
 */
 int main_(MainData* data)
{
    switch (data->token.type)
    {
        case T_IF:
        case T_WHILE:
        case T_ID:
        case T_INPUTS:
        case T_INPUTI:
        case T_INPUTF:
        case T_PRINT:
        case T_LENGTH:
        case T_SUBSTR:
        case T_ORD:
        case T_CHR:
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            /// pravidlo 8: <main> -> <code> <main>
            code(data);
            return main_(data);

        case T_EOL:
            ///pravidlo 36: <main> -> EOL<main>
            return SYNTAX_OK;

        case T_PASS:
            return code(data);

        case T_DEDENT:
            /// pravidlo 9: <main> -> e
            return SYNTAX_OK;

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Function process code: calls of iner functions, if, while and expressions.
 * @param data
 * @return
 */
 int code(MainData* data)
{
    switch (data->token.type)
    {
        case T_INPUTS:
        case T_INPUTI:
        case T_INPUTF:
        case T_PRINT:
        case T_LENGTH:
        case T_SUBSTR:
        case T_ORD:
        case T_CHR:
            /// pravidlo 10: <code> -> <inner_func> EOL
            gen_code_from_line(function_call);
            inner_func(data);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;

        case T_INT:
        case T_FLOAT:
        case T_STRING:
            /// pravidlo 11: <code> -> <EXPR> EOL

            ///samostatně stojící výraz
            expression(data);

            CHECK_TYPE(T_EOL);
            return SYNTAX_OK;

        case T_IF:
            /// pravidlo 12: <code> -> if <expr> : EOL INDENT <main> DEDENT else : EOL INDENT <main>  EOL DEDENT
            gen_code_from_line(if_line);
            GET_TOKEN();
            expression(data);

            CHECK_TYPE(T_COLON);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            GET_TOKEN();
            CHECK_TYPE(T_INDENT);
            GET_TOKEN();
            main_(data);
            if(data->token.type != T_DEDENT)
            {
                CHECK_TYPE(T_EOL);
            }
            GET_TOKEN();
            while(data->token.type != T_DEDENT)
            {
                main_(data);
                CHECK_TYPE(T_EOL);
                GET_TOKEN();
            }
           CHECK_TYPE(T_DEDENT);

            GET_TOKEN();
            while(data->token.type == T_DEDENT)
            {
                GET_TOKEN();
            }
         
            CHECK_TYPE(T_ELSE);
            gen_code_from_line(else_line);
            GET_TOKEN();

            CHECK_TYPE(T_COLON);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);
 
            GET_TOKEN();
            CHECK_TYPE(T_INDENT);
            GET_TOKEN();

            main_(data);
 
            if(data->token.type != T_DEDENT)
            {
            CHECK_TYPE(T_EOL);
            }
            GET_TOKEN();
            while(data->token.type != T_DEDENT)
            {
                main_(data);
                CHECK_TYPE(T_EOL);
                GET_TOKEN();
            }

            gen_code_from_line(dedent);

            return SYNTAX_OK;

        case T_WHILE:
            /// pravidlo 13: <code> -> while <expr> : EOL INDENT <main>   EOL DEDENT
            
            GET_TOKEN();
            gen_code_from_line(while_line);
            expression(data);

            CHECK_TYPE(T_COLON);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            GET_TOKEN();
            CHECK_TYPE(T_INDENT);
            GET_TOKEN();
            main_(data);
            CHECK_TYPE(T_EOL);
            GET_TOKEN();
            while(data->token.type != T_DEDENT)
            {
                main_(data);
                CHECK_TYPE(T_EOL);
                GET_TOKEN();
            }

            gen_code_from_line(dedent);

            return SYNTAX_OK;

        case T_ID:
            /// pravidlo 14: <code> -> ID <identif>
            data->second_token = data->token;
            GET_TOKEN();
            identif(data);
            return SYNTAX_OK;

        case T_PASS:
            ///pravidlo 37: <main> -> EOL<main>
            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;
        case T_EOL:
            return SYNTAX_OK;
        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}

/**
 * Progrm goes there after ID token is got from scanner.
 * @param data
 * @return
 */
 int identif(MainData* data)
{/// ID
    switch (data->token.type)
    {
        case T_EQ_COMP:
        case T_NEQ:
        case T_MORE:
        case T_MORE_EQ:
        case T_LESS:
        case T_LESS_EQ:
        case T_ADD:
        case T_SUB:
        case T_MUL:
        case T_DIV:
            /// pravidlo 15: <identif> -> EXPR EOL

            ///MÁME ULOŽEN 1 TOKEN
            ///SAMOSTATNĚ STOJÍCÍ VÝRAZ
            expression(data);
            return SYNTAX_OK;

        case T_LBRACK:
            /// pravidlo 16: <identif> -> (<call_func_params>) EOL

            gen_code_from_line(function_call);
            GET_TOKEN();
            call_func_params(data);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;

        case T_EQ_ASSIG:
        /// rule 17: <identif> -> = <ins>

            GET_TOKEN();
            return ins(data);

        case T_EOL:
            return SYNTAX_OK;
        default:
            errSyn();
            return SYNTAX_ERROR;
    }

}


/**
 * Program goes there after ID token and = token.
 * @param data
 * @return
 */
 int ins(MainData* data)
{///ID =
    switch (data->token.type)
    {
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            /// rule 18: <ins> -> EXPR EOL
           
            gen_code_from_line(assignment);
            expression(data);

            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;

        case T_ID:
            /// pravidlo 19: <ins> -> ID <ins_id>
            data->second_token = data->token;
            GET_TOKEN();

            return ins_id(data);

        case T_INPUTI:
        case T_INPUTF:
        case T_INPUTS:
            gen_code_from_line(function_call_with_assignment);
            inner_func(data);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Program goes there after ID, = and ID tokens.
 * @param data
 * @return
 */
 int ins_id(MainData* data)
{///ID = ID
    switch (data->token.type)
    {
        case T_LBRACK:
            /// rule 20: <ins_id> -> <call_func_params> EOL
            gen_code_from_line(function_call_with_assignment);
            GET_TOKEN();
            call_func_params(data);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);
            ///second_token = function ID

            return SYNTAX_OK;

        case T_EQ_COMP:
        case T_NEQ:
        case T_MORE:
        case T_MORE_EQ:
        case T_LESS:
        case T_LESS_EQ:
        case T_ADD:
        case T_SUB:
        case T_MUL:
        case T_DIV:
            /// pravidlo 21: <ins_id> -> EXPR EOL

            gen_code_from_line(assignment);
            data->third_token = data->second_token;
            data->second_token = data->token;

            expression(data);

            CHECK_TYPE(T_EOL);
            return SYNTAX_OK;

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Function process first param of called function.
 * @param data
 * @return
 */
 int call_func_params(MainData* data)
{
    switch (data->token.type)
    {
        case T_ID:
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            /// pravidlo 22: <call_func_params> ->  ID <call_func_param_x>

            GET_TOKEN();

            call_func_param_x(data);

            return SYNTAX_OK;

        case T_RBRACK:
            /// pravidlo 23: <call_func_param_x> -> e
            return SYNTAX_OK;


        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Function process params of called function.
 * @param data
 * @return
 */
 int call_func_param_x(MainData* data)
{
    switch (data->token.type)
    {
        case T_COMMA:
            /// pravidlo 24: <call_func_param_x> -> , ID <call_func_param_x>
            GET_TOKEN();
            if (data->token.type == T_ID || data->token.type == T_STRING || data->token.type == T_INT || data->token.type == T_FLOAT )
            {
            }
            else
            {
                errSyn();
                return SYNTAX_ERROR;
            }

            GET_TOKEN();
            return call_func_param_x(data);

        case T_RBRACK:
            /// pravidlo 25: <call_func_param_x> -> e

            return SYNTAX_OK;

        default:
            return SYNTAX_ERROR;
    }
}


/**
 * Function process buildin functions.
 * @param data
 * @return
 */
 int inner_func(MainData* data)
{

    switch (data->token.type)
    {
        case T_INPUTS:

            /// pravidlo 26: <inner_func> -> inputs ()
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            CHECK_TYPE(T_RBRACK);


            return SYNTAX_OK;

        case T_INPUTI:
            /// pravidlo 28: <inner_func> -> inputi ()
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            CHECK_TYPE(T_RBRACK);

            return SYNTAX_OK;

        case T_INPUTF:
            /// pravidlo 29: <inner_func> -> inputf ()

            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            CHECK_TYPE(T_RBRACK);

            return SYNTAX_OK;


        case T_PRINT:
            ///pravidlo 30: <inner_func> -> print (STRING <term> )

            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token.type == T_ID || data->token.type == T_STRING || data->token.type == T_INT || data->token.type == T_FLOAT )
            {
            }
            else
            {
                errSyn();
                return SYNTAX_ERROR;
            }
            GET_TOKEN();
            term(data);
            return SYNTAX_OK;

        case T_LENGTH:
            ///pravidlo 31: <inner_func> -> length (STRING  )

            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token.type == T_ID || data->token.type == T_STRING)
            {

                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }

        case T_SUBSTR:
            ///pravidlo 32: <inner_func> -> substr (STRING, INT, INT )

            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token.type == T_ID || data->token.type == T_STRING)
            {
                GET_TOKEN();
                CHECK_TYPE(T_COMMA);
                GET_TOKEN();
                if (data->token.type == T_ID || data->token.type == T_INT)
                {

                }
                else
                {
                    errSyn();
                    return SYNTAX_ERROR;
                }

                GET_TOKEN();
                CHECK_TYPE(T_COMMA);
                GET_TOKEN();

                if (data->token.type == T_ID || data->token.type == T_INT)
                {

                }
                else
                {
                    errSyn();
                    return SYNTAX_ERROR;
                }

                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }


        case T_ORD:
            ///pravidlo 33: <inner_func> -> ord (STRING, INT )
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token.type == T_ID || data->token.type == T_STRING)
            {
                GET_TOKEN();
                CHECK_TYPE(T_COMMA);
                GET_TOKEN();

                if (data->token.type == T_ID || data->token.type == T_INT)
                {

                }
                else
                {
                    errSyn();
                    return SYNTAX_ERROR;
                }


                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }

        case T_CHR:
            ///pravidlo 34: <inner_func> -> chr (INT )
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token.type == T_ID || data->token.type == T_INT)
            {
                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Handles terms in function print.
 * Program goes there, if print have 2 or more terms.
 * @param data
 * @return
 */
 int term(MainData* data)
{
    switch (data->token.type)
    {
        case T_COMMA:
            /// pravidlo 35: <term> -> "," STRING <term>
            GET_TOKEN();
            if (data->token.type == T_ID || data->token.type == T_STRING || data->token.type == T_INT || data->token.type == T_FLOAT)
            {

            }
            else
            {
                errSyn();
                return SYNTAX_ERROR;
            }
            GET_TOKEN();
            return term(data);

        case T_RBRACK:
            /// pravidlo 36: <term> -> e
            return SYNTAX_OK;
        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


/**
 * Main function of parser.
 * @return SYNTAX_OK when no errors
 */
int parse()
{
    init_symtable(255);
    int result;

    MainData *data;
    data = malloc(sizeof(MainData));

    data->token = *token_init();
    data->ptoken = &data->token;
    data->second_token = *token_init();
    data->second_token.type = T_NONE;
    data->third_token = *token_init();
    data->third_token.type = T_NONE;

    data->table = symtable_init(100);
    data->function_name = NULL;


    result = main_body(data);
    return result;
}
