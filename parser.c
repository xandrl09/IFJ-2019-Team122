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

            //print_stack();
            //create_insert_symbol(data->second_token.data->string, FUNCTION,  NULL);//jen ojeb v second_token zadna hodnota neni
            //printf("%d\n",is_in_stack(data->token.data->string));
            /// pokud již byla funkce definována fixme
//            if(is_in_stack( data->ptoken->data->string))
//            {
//                errSemDef();
//            }
//            /// uložení funkce do symtable
//            else
//            {
//                create_insert_symbol(data->ptoken->data->string, FUNCTION,  NULL);
//                data->function_name = data->ptoken->data->string;
//            }

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

            //CHECK_TYPE(T_DEDENT);
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
            gen_code_from_line(return_line);
            expression(data);
            return main_func(data);

        case T_DEDENT:
            gen_code_from_line(dedent);
            /// pravidlo 40: <main_func> -> e
            return SYNTAX_OK;

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


int def_func_params(MainData* data)
{
    switch (data->token.type)
    {
        case T_ID:
            /// pravidlo 4: <def_func_params> -> "ID" <func_param_x>
            insert_param( data->function_name,  data->ptoken->data->string, STRING);//fixme

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


int func_param_x(MainData* data)
{
    switch (data->token.type)
    {
        case T_COMMA:
            /// pravidlo 6: <func_param_x> -> "," "ID" <func_param_x>

            GET_TOKEN();
            CHECK_TYPE(T_ID);
            insert_param( data->function_name,  data->ptoken->data->string, STRING);//fixme

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
            gen_code_from_line(dedent);
            return SYNTAX_OK;

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


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
            while(data->token.type == T_DEDENT)//TODO DEDENT PRO VNORENY IF
            {
                GET_TOKEN();
            }
            gen_code_from_line(else_line);
            CHECK_TYPE(T_ELSE);

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
            //CHECK_TYPE(T_DEDENT);

            return SYNTAX_OK;

        case T_WHILE:
            /// pravidlo 13: <code> -> while <expr> : EOL INDENT <main>   EOL DEDENT
            gen_code_from_line(while_line);
            GET_TOKEN();
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
            //CHECK_TYPE(T_DEDENT);
            return SYNTAX_OK;

        case T_ID:
            /// pravidlo 14: <code> -> ID <identif>
            data->second_token = data->token;
            GET_TOKEN();
            return identif(data);


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

/// ID
int identif(MainData* data)
{
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

            call_func_params(data);

            //GET_TOKEN();
            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;

        case T_EQ_ASSIG:
            /// rule 17: <identif> -> = <ins>
//            if(symtable_search(data->table, data->second_token->data->string) == false)
//            {
//            }
//
//            data->table->hash_arr = symtable_lookup_add(data->table, data->second_token->data->string);
//            //data->op = new_operand(data->table->sym);
//
//            /// second_token = kam se prirazuje
//            data->result = data->second_token;
//            data->second_token->type = T_NONE;
//

            GET_TOKEN();
            return ins(data);

        case T_EOL:
            return SYNTAX_OK;
        default:
            errSyn();
            return SYNTAX_ERROR;
    }

}


///ID =
int ins(MainData* data)
{
    switch (data->token.type)
    {
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            // rule 18: <ins> -> EXPR EOL
            gen_code_from_line(assignment);

            expression(data);
            CHECK_TYPE(T_EOL);

            return SYNTAX_OK;
        case T_ID:
            // pravidlo 19: <ins> -> ID <ins_id>
            data->second_token = data->token;
            GET_TOKEN();

            return ins_id(data);

        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


///ID = ID
int ins_id(MainData* data)
{
    switch (data->token.type)
    {
        case T_LBRACK:
            // rule 20: <ins_id> -> <call_func_params> EOL
            gen_code_from_line(function_call_with_assignment);

            ///second_token = function ID
//            data->func_id = data->second_token;
//            data->second_token->type = T_NONE;


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

            if(data->token.type == T_ID)
            {
                /// kontrola zda je parametr v parametrech funkce
//                 if(arg_search(data->table, data->func_id->data->string, data->token->data->string) == false)return SEM_ERR_WRONG_FUNC_PARAMS ;
//
//                 data->number_of_params = 1;
                GET_TOKEN();
                call_func_param_x(data);

                ///kontrola poctu parametru
//                Symbol* s = func_add(data->table, data->func_id->data->string);
//                int i = arg_count(s);
//                if(data->number_of_params != i)return SEM_ERR_WRONG_FUNC_PARAMS;
            }
            else
            {
                GET_TOKEN();
                call_func_params(data);
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


int call_func_params(MainData* data)
{
    switch (data->token.type)
    {
        case T_ID:
            // pravidlo 22: <call_func_params> ->  ID <call_func_param_x>

            /// kontrola zda je parametr v parametrech funkce
            //if(arg_search(data->table, data->func_id->data->string, data->token->data->string) == false)return SEM_ERR_WRONG_FUNC_PARAMS ;

            GET_TOKEN();
            //data->number_of_params = 1;
            call_func_param_x(data);


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
            errSyn();
            return SYNTAX_ERROR;
    }
}


int call_func_param_x(MainData* data)
{
    switch (data->token.type)
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


int inner_func(MainData* data)
{
    gen_code_from_line(function_call);
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
            return term(data);

        case T_RBRACK:
            /// pravidlo 36: <term> -> e
            return SYNTAX_OK;
        default:
            errSyn();
            return SYNTAX_ERROR;
    }
}


int parse()
{
    //extern table_stack *stack;
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

    data->table = symtable_init(100); // FIXME
    data->function_name = NULL;
    //(data)->number_of_params = 0;

    result = main_body(data);
    return result;
}