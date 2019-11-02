/**
 * Subject: IFJ & IAL
 *
 * Project: Implementation of project IFJ19.
 *
 * @brief Parser.
 *
 * @author Ondřej Andrla <xandrl09@stud.fit.vutbr.cz>
 */

#pragma once

#include <stdio.h>

#include "parser.h"
//#include "lexan.h"


#define GET_TOKEN()\
if ((result = getParserToken(data->token)) != SCANNER_TOKEN_OK)\
return result

#define CHECK_TYPE(_type)											\
if (data->token->type != (_type)) return SYNTAX_ERROR



// ==================================================================
// jednotlive funkce odpovidajici jednotlivym nonterminalum gramatiky
// ==================================================================

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



static int main_body(MainData* data)
{
    int result;
    GET_TOKEN();

    switch(data->token->type)
    {
        case T_DEF:
            ///Pravidlo 1: def ID ( <func_params> ) EOL: INDENT <main_func> DEDENT <main_body>

            //data->op = new_operator(BEGIN_FUNCTION_DEFINITION);
            //(data->prog, data->op);
            GET_TOKEN();
            CHECK_TYPE(T_ID);

            data->func_id = data->token;

//            // TODO ZKONTROLOVAT
//            /// pokud již byla funkce definována
//            if(sym_search(data->table, data->token->data->string))
//            {
//                return SEM_ERR_UNDEFINED_VAR;
//            }
//            /// uložení funkce do symtable
//            else
//            {
//                data->table->hash_arr = func_add(data->table, data->token->data->string);
//
//                //data->op = new_operand(data->table->sym);
//                //append_op(data->prog, data->op);
//            }

            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);

            GET_TOKEN();
            result = def_func_params(data);
            if (result != SYNTAX_OK) return result;

            GET_TOKEN();
            CHECK_TYPE(T_EOL);
            GET_TOKEN();
            CHECK_TYPE(T_COLON);
            GET_TOKEN();
            CHECK_TYPE(T_INDENT);

            GET_TOKEN();
            data->in_function = true;
            result = main_func(data);
            if (result != SYNTAX_OK) return result;
            data->in_function = false;

            CHECK_TYPE(T_DEDENT);
            //data->op = new_operator(END);
            //(data->prog, data->op);

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
            // Pravidlo 2 <main_body> -> <code> <main_body>

            result = code(data);
            if (result != SYNTAX_OK) return result;

            return main_body(data);

        case T_EOF:
            // Pravidlo 3 <main_body> -> e
           // data->op = new_operator(PROG_END);
            //(data->prog, data->op);
            return SYNTAX_OK;

        default:
            return SYNTAX_ERROR;
    }
}


static int main_func(MainData* data)
{
    int result;
    int i;

    switch (data->token->type)
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
        case T_EOL:
        case T_PASS:
            // pravidlo 38: <main> -> <code> <main>

            result = code(data);
            if (result != SYNTAX_OK) return result;
            return main_func(data);

        case T_RETURN:
            // pravidlo 39: return <expr> <main_func>
            i = expression(data);
            if(i != 0) return i;

            return main_func(data);
        case T_DEDENT:
            // pravidlo 40: <main_func> -> e
            return SYNTAX_OK;

        default:
            return SYNTAX_ERROR;
    }
}


static int def_func_params(MainData* data)
{
    int result;

    switch (data->token->type)
    {
        case T_ID:
            // pravidlo 4: <def_func_params> -> "ID" <func_param_x>

            ///sem se dostaneme pouze pokud funkce nebyla definovana

//            //data->table->sym = symtable_lookup_add(data->table->sym, data->token->data.string);	//získání adresy symbolu funkce?
//            data->table->hash_arr = arg_add(data->table, data->func_id->data->string , data->token->data->string);
//            //data->op = new_operand(data->table->sym);
//           // append_op(data->prog, data->op);

            GET_TOKEN();
            result = func_param_x(data);
            if (result != SYNTAX_OK) return result;

            return SYNTAX_OK;

        case T_RBRACK:
            // pravidlo 5: <def_func_params> -> e
            return SYNTAX_OK;
        default:
            return SYNTAX_ERROR;

    }
}


static int func_param_x(MainData* data)
{
    int result;

    switch (data->token->type)
    {
        case T_COMMA:
            // pravidlo 6: <func_param_x> -> "," "ID" <func_param_x>

            GET_TOKEN();
            CHECK_TYPE(T_ID);

            ///sem se dostaneme pouze pokud funkce nebyla definovana

//            // ULOŽENÍ PARAMETRU FUNKCE
//            data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
//            //data->op = new_operand(data->table->sym);
//            //append_op(data->prog, data->op);

            return func_param_x(data);

        case T_RBRACK:
             // pravidlo 7: <func_param_x> -> e
             return SYNTAX_OK;
        default:
            return SYNTAX_ERROR;
    }
}


static int main_(MainData* data)
{
    int result;

    switch (data->token->type)
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
        case T_EOL:
        case T_PASS:
            // pravidlo 8: <main> -> <code> <main>

            result = code(data);
            if (result != SYNTAX_OK) return result;
            return main_(data);

        case T_DEDENT:
            // pravidlo 9: <main> -> e
            return SYNTAX_OK;

        default:
            return SYNTAX_ERROR;
    }
}


static int code(MainData* data)
{
    int result;
    int i;

    switch (data->token->type)
    {
        case T_INPUTS:
        case T_INPUTI:
        case T_INPUTF:
        case T_PRINT:
        case T_LENGTH:
        case T_SUBSTR:
        case T_ORD:
        case T_CHR:
            // pravidlo 10: <code> -> <inner_func> EOL
            result = inner_func(data);
            if (result != SYNTAX_OK) return result;

            GET_TOKEN();
            CHECK_TYPE(T_EOL);
            return SYNTAX_OK;

        case T_INT:
        case T_FLOAT:
        case T_STRING:
            // pravidlo 11: <code> -> <EXPR> EOL

            ///samostatně stojící výraz

            //data->op = new_operator(BEGIN_EXPR);
            //(data->prog, data->op);
            i = expression(data);
            if(i != 0) return i;
            //data->op = new_operator(END);
            //(data->prog, data->op);

            CHECK_TYPE(T_EOL);
            return SYNTAX_OK;

        case T_IF:
            // pravidlo 12: <code> -> if <expr> : EOL INDENT <main> DEDENT else : EOL INDENT <main> DEDENT  EOL

            //data->op = new_operator(BEGIN_IF);
            //(data->prog, data->op);

            GET_TOKEN();

            //data->op = new_operator(BEGIN_EXPR);
            //(data->prog, data->op);
            i = expression(data);
            if(i != 0) return i;
            //data->op = new_operator(END);
            //(data->prog, data->op);

            CHECK_TYPE(T_COLON);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            GET_TOKEN();
            CHECK_TYPE(T_INDENT);
            GET_TOKEN();
            result = main_(data);
            if (result != SYNTAX_OK) return result;
            CHECK_TYPE(T_DEDENT);

            GET_TOKEN();
            CHECK_TYPE(T_ELSE);
            //data->op = new_operator(ELSE);
            //(data->prog, data->op);

            GET_TOKEN();
            CHECK_TYPE(T_COLON);
            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            GET_TOKEN();
            CHECK_TYPE(T_INDENT);
            GET_TOKEN();
            result = main_(data);
            if (result != SYNTAX_OK) return result;
            CHECK_TYPE(T_DEDENT);
            //data->op = new_operator(END);
            //(data->prog, data->op);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);
            return SYNTAX_OK;

        case T_WHILE:
            // pravidlo 13: <code> -> while <expr> : EOL INDENT <main> DEDENT  EOL
            //data->op = new_operator(BEGIN_WHILE);
            //(data->prog, data->op);

            GET_TOKEN();

            //data->op = new_operator(BEGIN_EXPR);
            //(data->prog, data->op);
            i = expression(data);
            if(i != 0) return i;
            //data->op = new_operator(END);
            //(data->prog, data->op);
            CHECK_TYPE(T_COLON);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);

            GET_TOKEN();
            CHECK_TYPE(T_INDENT);
            GET_TOKEN();
            result = main_(data);
            if (result != SYNTAX_OK) return result;

            CHECK_TYPE(T_DEDENT);
            //data->op = new_operator(END);
            //(data->prog, data->op);

            GET_TOKEN();
            CHECK_TYPE(T_EOL);
            return SYNTAX_OK;

        case T_ID:
            // pravidlo 14: <code> -> ID <identif>
            data->second_token = data->token;
            GET_TOKEN();
            return identif(data);
        case T_EOL:
            //pravidlo 36: <main> -> EOL<main>
            return SYNTAX_OK;

        case T_PASS:
            //pravidlo 37: <main> -> EOL<main>
            GET_TOKEN();
            CHECK_TYPE(T_EOL);
            return SYNTAX_OK;

        default:
            return SYNTAX_ERROR;
    }
}

/// ID
static int identif(MainData* data)
{
    int result;
    int i;

    switch (data->token->type)
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
            // pravidlo 15: <identif> -> EXPR EOL

            ///MÁME ULOŽEN 1 TOKEN

            ///SAMOSTATNĚ STOJÍCÍ VÝRAZ

            //data->op = new_operator(BEGIN_EXPR);
            //(data->prog, data->op);

            i = expression(data);
            if(i != 0) return i;
            //data->op = new_operator(END);
            //(data->prog, data->op);
            return SYNTAX_OK;

        case T_LBRACK:
            // pravidlo 16: <identif> -> (<call_func_params>) EOL

            //DONE pro kontrolu jesi je funkce deklarovan stačí func_search, což se kontroluje 3 řádky níže
            /// ID je v symtable
//            if(symtable_search(data->table, data->second_token->data->string) )
//            {
//                /// je to funkce
//                if(func_search(data->table, data->second_token->data->string))
//                {
//                    //volani funkce TODO
//
//                }
//                    ///je to proměná
//                else
//                {
//                    return SEM_ERR_UNDEFINED_VAR;
//                }
//            }
//            else
//            {
//                ///  u funkce je  chyba pokud je volaná nedeklarovaná z hlavního těla
//                /// je volana z funkce
//                if(data->in_function)
//                {
//                    //todo zapamatovat si že nebyla inicializovaná a na konci zkontrolovat jestli byla v průběhu inicializovana
//                    // pokud nebyla inicializovana tak pak nakonec:  return SEM_ERR_UNDEFINED_VAR;
//                }
//                else
//                {
//                    return SEM_ERR_UNDEFINED_VAR;
//                }
//            }

            result = call_func_params(data);
            if (result != SYNTAX_OK) return result;

            //GET_TOKEN();
            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;

        case T_EQ_ASSIG:
        /// rule 17: <identif> -> = <ins>
//            if(symtable_search(data->table, data->second_token->data->string) == false)
//            {
//                //data->op = new_operator(DEFVAR);
//                //(data->prog, data->op);
//            }
//
//            data->table->hash_arr = symtable_lookup_add(data->table, data->second_token->data->string);
//            //data->op = new_operand(data->table->sym);
//            //(data->prog, data->op);
//
//            /// second_token = kam se prirazuje
//            data->result = data->second_token;
//            data->second_token->type = T_NONE;
//
//            //data->op = new_operator(ASSIGN);
//            //(data->prog, data->op);
            GET_TOKEN();
            return ins(data);

        default:
            return SYNTAX_ERROR;
    }

}


///ID =
static int ins(MainData* data)
{
    int result;
    int i;

    switch (data->token->type)
    {
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            // rule 18: <ins> -> EXPR EOL

            //data->op = new_operator(BEGIN_EXPR);
            //(data->prog, data->op);
            i = expression(data);
            if(i != 0) return i;
            //data->op = new_operator(END);
            //(data->prog, data->op);

            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;
        case T_ID:
            // pravidlo 19: <ins> -> ID <ins_id>
            data->second_token = data->token;
            GET_TOKEN();

            return ins_id(data);

        default:
            return SYNTAX_ERROR;
    }
}


///ID = ID
static int ins_id(MainData* data)
{
    int result;

    switch (data->token->type)
    {
        case T_LBRACK:
            // rule 20: <ins_id> -> <call_func_params> EOL

            ///second_token = function ID
            data->func_id = data->second_token;
            data->second_token->type = T_NONE;


            /// ID je v symtable
//            if(symtable_search(data->table, data->second_token->data->string))
//            {
//                /// je to funkce
//                if(func_search(data->table, data->second_token->data->string))
//                {
//                    //volani funkce
//
//                }
//                    ///je to proměná
//                else
//                {
//                    return SEM_ERR_UNDEFINED_VAR;
//                }
//            }
//            else
//            {
//                ///  u funkce je  chyba pokud je volaná nedeklarovaná z hlavního těla
//                /// je volana z funkce
//                if(data->in_function)
//                {
//                    //todo zapamatovat si že nebyla inicializovaná a na konci zkontrolovat jestli byla v průběhu inicializovana
//                    // pokud nebyla inicializovana tak pak nakonec:  return SEM_ERR_UNDEFINED_VAR;
//                }
//                else
//                {
//                    return SEM_ERR_UNDEFINED_VAR;
//                }
//            }

            if(data->token->type == T_ID)
            {
                /// kontrola zda je parametr v parametrech funkce
//                 if(arg_search(data->table, data->func_id->data->string, data->token->data->string) == false)return SEM_ERR_WRONG_FUNC_PARAMS ;
//
//                 data->number_of_params = 1;
                GET_TOKEN();
                result = call_func_param_x(data);
                if (result != SYNTAX_OK) return result;

                ///kontrola poctu parametru
//                Symbol* s = func_add(data->table, data->func_id->data->string);
//                int i = arg_count(s);
//                if(data->number_of_params != i)return SEM_ERR_WRONG_FUNC_PARAMS;
            }
            else
            {
                GET_TOKEN();
                result = call_func_params(data);
                if (result != SYNTAX_OK) return result;
            }


            GET_TOKEN();
            CHECK_TYPE(T_EOL);

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
            // pravidlo 21: <ins_id> -> EXPR EOL

            data->third_token = data->second_token;
            data->second_token = data->token;

            //data->op = new_operator(BEGIN_EXPR);
            //(data->prog, data->op);
            int i = expression(data);
            if(i != 0) return i;
            //data->op = new_operator(END);
            //(data->prog, data->op);

            CHECK_TYPE(T_EOL);
            return SYNTAX_OK;

        default:
            return SYNTAX_ERROR;
    }
}


static int call_func_params(MainData* data)
{
    int result;

    switch (data->token->type)
    {
        case T_ID:
            // pravidlo 22: <call_func_params> ->  ID <call_func_param_x>

            /// kontrola zda je parametr v parametrech funkce
            //if(arg_search(data->table, data->func_id->data->string, data->token->data->string) == false)return SEM_ERR_WRONG_FUNC_PARAMS ;

            GET_TOKEN();
            //data->number_of_params = 1;
            result = call_func_param_x(data);
            if (result != SYNTAX_OK) return result;

            ///kontrola poctu parametru
//            Symbol* s = func_add(data->table, data->func_id->data->string);
//            int i = arg_count(s);
//            if(data->number_of_params != i)return SEM_ERR_WRONG_FUNC_PARAMS;

            CHECK_TYPE(T_RBRACK);
            return SYNTAX_OK;

        case T_RBRACK:
            /// pravidlo 23: <call_func_param_x> -> e

            /// fuknce již nemá další parametry
            return SYNTAX_OK;

//            /// ID první parametr funkce
//        case T_ID:
//            // pravidlo 23: <call_func_params> ->  ID <call_func_param_x>
//
//
//            /// kontrola zda je parametr v parametrech funkce
//            if(arg_search(data->table, data->func_id->data->string, data->token->data->string) == false)return SEM_ERR_WRONG_FUNC_PARAMS ;
//
//            GET_TOKEN();
//            data->number_of_params = 1;
//            result = call_func_param_x(data);
//            if (result != SYNTAX_OK) return result;
//
//            ///kontrola poctu parametru
//             s = func_add(data->table, data->func_id->data->string);
//             i = arg_count(s);
//            if(data->number_of_params != i)return SEM_ERR_WRONG_FUNC_PARAMS;
//
//            return SYNTAX_OK;

        default:
            return SYNTAX_ERROR;
    }
}


static int call_func_param_x(MainData* data)
{
    int result;

    switch (data->token->type)
    {
        case T_COMMA:
            /// pravidlo 24: <call_func_param_x> -> , ID <call_func_param_x>
            GET_TOKEN();
            CHECK_TYPE(T_ID);
            //data->number_of_params ++;

            /// kontrola zda je parametr v parametrech funkce
            //if(arg_search(data->table, data->func_id->data->string, data->token->data->string) == false)return SEM_ERR_WRONG_FUNC_PARAMS ;

            GET_TOKEN();
            return call_func_param_x(data);

        case T_RBRACK:
            /// pravidlo 25: <call_func_param_x> -> e

            /// fuknce již nemá další parametry
            return SYNTAX_OK;

        default:
            return SYNTAX_ERROR;
    }
}


static int inner_func(MainData* data)
{
    int result;

    switch (data->token->type)
    {
        case T_INPUTS:
            // pravidlo 26: <inner_func> -> inputs ()
            //data->op = new_operator(BEGIN_FUNCTION_CALL);
            //(data->prog, data->op);
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            CHECK_TYPE(T_RBRACK);

            //data->op = new_operator(INPUTS);
            //(data->prog, data->op);
            //data->op = new_operator(END);
            //(data->prog, data->op);
            return SYNTAX_OK;

        case T_INPUTI:
            // pravidlo 28: <inner_func> -> inputi ()
            //data->op = new_operator(BEGIN_FUNCTION_CALL);
            //(data->prog, data->op);
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            CHECK_TYPE(T_RBRACK);

            //data->op = new_operator(INPUTI);
            //(data->prog, data->op);
            //data->op = new_operator(END);
            //(data->prog, data->op);
            return SYNTAX_OK;

        case T_INPUTF:
            // pravidlo 29: <inner_func> -> inputf ()
            //data->op = new_operator(BEGIN_FUNCTION_CALL);
            //(data->prog, data->op);
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            CHECK_TYPE(T_RBRACK);

            //data->op = new_operator(INPUTF);
            //(data->prog, data->op);
            //data->op = new_operator(END);
            //(data->prog, data->op);
            return SYNTAX_OK;


        case T_PRINT:
            //pravidlo 30: <inner_func> -> print (STRING <term> )

            //data->op = new_operator(BEGIN_FUNCTION_CALL);
            //(data->prog, data->op);

            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);

            GET_TOKEN();
            if (data->token->type == T_ID) {
                // operand
                //data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
                // data->op = new_operand(data->table->sym);
                //(data->prog, data->op);

                GET_TOKEN();
                result = term(data);
                if (result != SYNTAX_OK) return result;

                   // data->op = new_operator(PRINT);
                    //(data->prog, data->op);
                    //data->op = new_operator(END);
                    //(data->prog, data->op);

            } else if (data->token->type == T_STRING || data->token->type == T_INT || data->token->type == T_FLOAT) {
                // operand
                // data->symb = new_symbol();
//                if (data->token->type == T_STRING) {
//                    data->symb->type = STRING;
//                    data->symb->data->string = data->token->data->string;
//                } else if (data->token->type == T_INT) {
//                    data->symb->type = INT;
//                    data->symb->data->integer = data->token->data->u_int;
//                } else {
//                    data->symb->type = FLOAT;
//                    data->symb->data->flp = data->token->data->u_double;
//                }

                //data->op = new_operand(data->symb);
                //(data->prog, data->op);
                GET_TOKEN();
                result = term(data);
                if (result != SYNTAX_OK) return result;

                    //data->op = new_operator(PRINT);
                    //(data->prog, data->op);
                   // data->op = new_operator(END);
                    //(data->prog, data->op);

            } else{
                return SYNTAX_ERROR;
            }
            GET_TOKEN();
            result = term(data);
            if (result != SYNTAX_OK) return result;

            GET_TOKEN();
            if (data->token->type != T_RBRACK) return SYNTAX_ERROR;

                //data->op = new_operator(PRINT);
                //(data->prog, data->op);
                //data->op = new_operator(END);
                //(data->prog, data->op);
            return SYNTAX_OK;


        case T_LENGTH:
            //pravidlo 31: <inner_func> -> length (STRING  )

            //data->op = new_operator(BEGIN_FUNCTION_CALL);
            //(data->prog, data->op);
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token->type == T_ID)
            {
                //data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
                //data->op = new_operand(data->table->sym);
                //(data->prog, data->op);

                //data->op = new_operator(LENGTH);
                //(data->prog, data->op);
                //data->op = new_operator(END);
                //(data->prog, data->op);
                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }
            else if( data->token->type == T_STRING)
            {
//                data->symb = new_symbol();
//                data->symb->type = STRING;
//                data->symb->data->string = data->token->data->string;
                //data->op = new_operand(data->symb);
                //(data->prog, data->op);

                //data->op = new_operator(LENGTH);
                //(data->prog, data->op);
               // data->op = new_operator(END);
                //(data->prog, data->op);
                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }


        case T_SUBSTR:
            //pravidlo 32: <inner_func> -> substr (STRING, INT, INT )

            //data->op = new_operator(BEGIN_FUNCTION_CALL);
            //(data->prog, data->op);
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token->type == T_ID || data->token->type == T_STRING)
            {
//                if (data->token->type == T_ID)
//                {
//                    //data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
//                    //data->op = new_operand(data->table->sym);
//                    //(data->prog, data->op);
//                }
//                else
//                {
//                    //data->symb = new_symbol();
//                    data->symb->type = STRING;
//                    data->symb->data->string = data->token->data->string;
//                    //data->op = new_operand(data->symb);
//                    //(data->prog, data->op);
//                }

                GET_TOKEN();
                CHECK_TYPE(T_COMMA);
                GET_TOKEN();
                if (data->token->type == T_ID)
                {
                   // data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
                   // data->op = new_operand(data->table->sym);
                    //(data->prog, data->op);
                }
                else if ( data->token->type == T_INT)
                {
                   // data->symb = new_symbol();
//                    data->symb->type = INT;
//                    data->symb->data->integer = data->token->data->u_int;
                   // data->op = new_operand(data->symb);
                    //(data->prog, data->op);
                }
                else return SYNTAX_ERROR;

                GET_TOKEN();
                CHECK_TYPE(T_COMMA);
                GET_TOKEN();

                if (data->token->type == T_ID)
                {
                    //data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
                    //data->op = new_operand(data->table->sym);
                    //(data->prog, data->op);
                }
                else if ( data->token->type == T_INT)
                {
                    //data->symb = new_symbol();
//                    data->symb->type = INT;
//                    data->symb->data->integer = data->token->data->u_int;
                    //data->op = new_operand(data->symb);
                    //(data->prog, data->op);
                }
                else return SYNTAX_ERROR;

                //data->op = new_operator(SUBSTR);
                //(data->prog, data->op);
                //data->op = new_operator(END);
                //(data->prog, data->op);
                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }


        case T_ORD:
            //pravidlo 33: <inner_func> -> ord (STRING, INT )

            //data->op = new_operator(BEGIN_FUNCTION_CALL);
            //(data->prog, data->op);
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token->type == T_ID || data->token->type == T_STRING)
            {
//                if (data->token->type == T_ID)
//                {
//                  //  data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
//                   // data->op = new_operand(data->table->sym);
//                    //(data->prog, data->op);
//                }
//                else
//                {
//                    //data->symb = new_symbol();
//                    data->symb->type = STRING;
//                    data->symb->data->string = data->token->data->string;
//                    //data->op = new_operand(data->symb);
//                    //(data->prog, data->op);
//                }

                GET_TOKEN();
                CHECK_TYPE(T_COMMA);
                GET_TOKEN();

                if (data->token->type == T_ID)
                {
                 //   data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
                   // data->op = new_operand(data->table->sym);
                    //(data->prog, data->op);
                }
                else if ( data->token->type == T_INT)
                {
//                    data->symb = new_symbol();
//                    data->symb->type = INT;
//                    data->symb->data->integer = data->token->data->u_int;
                    //data->op = new_operand(data->symb);
                    //(data->prog, data->op);
                }
                else return SYNTAX_ERROR;

                //data->op = new_operator(ORD);
                //(data->prog, data->op);
               // data->op = new_operator(END);
                //(data->prog, data->op);
                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }



        case T_CHR:
            //pravidlo 34: <inner_func> -> chr (INT )

            //data->op = new_operator(BEGIN_FUNCTION_CALL);
            //(data->prog, data->op);
            GET_TOKEN();
            CHECK_TYPE(T_LBRACK);
            GET_TOKEN();
            if (data->token->type == T_ID || data->token->type == T_INT)
            {
//                if (data->token->type == T_ID)
//                {
//                    data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
//                   // data->op = new_operand(data->table->sym);
//                    //(data->prog, data->op);
//                }
//                else
//                {
//                   // data->symb = new_symbol();
//                    data->symb->type = INT;
//                    data->symb->data->integer = data->token->data->u_int;
//                    //data->op = new_operand(data->symb);
//                    //(data->prog, data->op);
//                }

               // data->op = new_operator(CHR);
                //(data->prog, data->op);
                //data->op = new_operator(END);
                //(data->prog, data->op);
                GET_TOKEN();
                CHECK_TYPE(T_RBRACK);
                return SYNTAX_OK;
            }

        default:
            return SYNTAX_ERROR;
    }
}


static int term(MainData* data)
{
    int result;

    switch (data->token->type)
    {
        case T_COMMA:
            // pravidlo 35: <term> -> "," STRING <term>
            GET_TOKEN();
            if (data->token->type == T_ID) {
                // operand
               // data->table->hash_arr = symtable_lookup_add(data->table, data->token->data->string);
               // data->op = new_operand(data->table->sym);
                //(data->prog, data->op);

            } else if (data->token->type == T_STRING || data->token->type == T_INT || data->token->type == T_FLOAT) {
                // operand
               // data->symb = new_symbol();

//                if (data->token->type == T_STRING) {
//                    data->symb->type = STRING;
//                    data->symb->data->string = data->token->data->string;
//                } else if (data->token->type == T_INT) {
//                    data->symb->type = INT;
//                    data->symb->data->integer = data->token->data->u_int;
//                } else {
//                    data->symb->type = FLOAT;
//                    data->symb->data->flp = data->token->data->u_double;
//                }

               // data->op = new_operand(data->symb);
                //(data->prog, data->op);

            } else
                return SYNTAX_ERROR;

            return term(data);

        case T_RBRACK:
            // pravidlo 36: <term> -> e
            return SYNTAX_OK;
        default:
            return SYNTAX_ERROR;
    }
}


//T_token *token_init ()
//{
//    struct T_token *token  = malloc(sizeof(T_token));
//    token->data = malloc(sizeof(T_tokenData));
//    return token;
//}


int parse()
{
    int result;

    MainData *data;
    //BST table;
        data = malloc(sizeof(MainData));


            (data)->token = token_init();
            //(data)->token->data->string == dyn_id_init() ;
            (data)->second_token = token_init();
    //(data)->second_token->data->string == dyn_id_init() ;
            (data)->third_token = token_init();

            (data)->func_id = token_init();
            //(data)->result = token_init();
            (data)->table = init_table(1); // FIXME
            (data)->symb = NULL;
            (data)->in_function = false;
            (data)->non_declared_function = false;
            //(data)->number_of_params = 0;


    //dyn_id string;
    //if (!dyn_id_init()) return ERROR_INTERNAL;
    //dyn_id_init();


        result = main_body(data);

   // dyn_id_destroy(&string);
 printf("%i",result);
    return result;
}
