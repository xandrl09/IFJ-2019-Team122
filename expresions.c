/**
 * @author Ondřej Andrla
 * Year 2019
 * Expressions
 */

//#pragma once

#include "expresions.h"
//#include "parser.h"
//#include "scanner.h"
#include "main.h"




#define FREE_RESOURCES_RETURN(_return)								\
		ex_stack_destroy(stack);									\
		return _return;												\




typedef enum
{
    I_EQU_NEQU,
    I_REL_OP,			/// 0 r
    I_PLUS_MINUS,		/// 1 +-
    I_MUL_DIV,		    /// 2 */ //
    I_LEFT_BRACKET,		/// 3 (
    I_RIGHT_BRACKET,	/// 4 )
    I_DATA,	            /// 5 i
    I_DOLLAR			/// 6 $

} Precedence_table_index_enum;

typedef enum
{
    S,    /// < SHIFT
    Q,    /// = EQUAL
    R,    /// > REDUCE
    E     /// # ERROR
} Precedence_table_sign_enum;



int precedence_table[7][7] =
        {
           //	  r |+- |*/ | ( | ) | i | $ |
                { E , S , S , S , R , S , R }, /// r (realtion operators) ==!= <= < >= >
                { R , R , S , S , R , S , R }, /// +-
                { R , R , R , S , R , S , R }, /// */
                { S , S , S , S , Q , S , E }, /// (
                { R , R , R , E , R , E , R }, /// )
                { R , R , R , E , R , E , R }, /// i (id, int, double, string)
                { S , S , S , S , E , S , E }  /// $
        };


/**
 * Function converts token type to symbol.
 *
 * @param token Pointer to token.
 * @return Returns dollar if symbol is not supported or converted symbol if symbol is supported.
 */
static symbol_enum get_symbol_from_token(T_token* token)
{
        switch (token->type)
        {
                case T_EQ_COMP:
                        return EQ_;
                case T_NEQ:
                        return NEQ;
                case T_LESS_EQ:
                        return LEQ;
                case T_LESS:
                        return LESS;
                case T_MORE_EQ:
                        return MEQ;
                case T_MORE:
                        return MORE;
                case T_ADD:
                        return PLUS;
                case T_SUB:
                        return MINUS;
                case T_MUL:
                        return MUL_;
                case T_DIV:
                        return DIV_;
                case T_LBRACK:
                        return LEFT_BRACKET_;
                case T_RBRACK:
                        return RIGHT_BRACKET_;
		default:
			switch(token->type)
			{
				case T_ID:
				        return ID;
				case T_INT:
				        return INT_;
				case T_FLOAT:
				        return FLOAT_;
				case T_STRING:
				        return STRING_;
				default:
				        return DOLLAR;
		                }
        }
}


/**
 * Function converts symbol to precedence table index.
 *
 * @param symbol Symbol to be converted.
 * @return Returns precedence table index.
 */
static Precedence_table_index_enum get_precedence_table_index(symbol_enum symbol)
{
        switch (symbol)
        {
                case PLUS:
                    return I_PLUS_MINUS;
                case MINUS:
                        return I_PLUS_MINUS;

                case MUL_:
                    return I_MUL_DIV;
                case DIV_:
                        return I_MUL_DIV;

                case EQ_:
                case NEQ:
                        return I_EQU_NEQU;

                case LEQ:
                    return I_REL_OP;
                case LESS:
                    return I_REL_OP;
                case MEQ:
                    return I_REL_OP;
                case MORE:
                        return I_REL_OP;

                case LEFT_BRACKET_:
                        return I_LEFT_BRACKET;

                case RIGHT_BRACKET_:
                        return I_RIGHT_BRACKET;

                case ID:
                    return I_DATA;
                case INT_:
                    return I_DATA;
                case FLOAT_:
                    return I_DATA;
                case STRING_:
                        return I_DATA;

                default:
                        return I_DOLLAR;
        }
}


/**
 * Function function counts number of symbols after stop symbol on stack.
 *
 * @param stop_found Pointer to bool variable which will be changed to true if stop was found else to false.
 * @return Number of characters after stop symbol. Is valid only when stop_found was set to true.
 */
static int num_of_symbols_after_stop(bool* stop_found, stack_exp *stack)
{
        list_exp* tmp = ex_stack_top(stack);
        int count = 0;

        while (tmp != NULL)
        {
                if (tmp->symbol != STOP)
                {
                        *stop_found = false;
                        count++;
                }
                else
                {
                        *stop_found = true;
                        break;
                }

                tmp = tmp->next;
        }

        return count;
}


/**
 * Function tests if symbols in parameters are valid according to rules.
 *
 * @param num Number of valid symbols in parameter.
 * @return NOT_A_RULE if no rule is found or returns rule which is valid.
 */
bool  test_rule(int num, list_exp* op1, list_exp* op2, list_exp* op3)
{
        switch (num)
        {
                case 1:
                        /// rule E -> i
                        if (op1->symbol == ID || op1->symbol == INT_ || op1->symbol == FLOAT_ || op1->symbol == STRING_)
				
                                return true;

                return false;

                case 3:
                        /// rule E -> (E)
                        if (op1->symbol == LEFT_BRACKET_ && op2->symbol == NON_TERM && op3->symbol == RIGHT_BRACKET_)
                                return true;

                if (op1->symbol == NON_TERM && op3->symbol == NON_TERM)
                {
                        switch (op2->symbol)
                        {
                                case PLUS:
                                case MINUS:
                                case MUL_:
                                case DIV_:
                                case EQ_:
                                case NEQ:
                                case LEQ:
                                case LESS:
                                case MEQ:
                                case MORE:
                                        return true;
                                /// invalid operator
                                default:
                                        return false;
                        }
                }
                return false;

                default:
                        return false;
        }

}


/**
 * Function reduces symbols after STOP symbol if rule for reducing is found.
 *
 * @param data Pointer to table.
 * @return Given exit code.
 */
static int reduce( stack_exp *stack)
{

        list_exp* op1 = NULL;
        list_exp* op2 = NULL;
        list_exp* op3 = NULL;

        bool rule;
        bool found = false;

        int count = num_of_symbols_after_stop(&found, stack);


        if (count == 1 && found)
        {
                op1 = stack->head;
                rule = test_rule(count, op1, NULL, NULL);
        }
        else if (count == 3 && found)
        {
                op1 = stack->head->next->next;
                op2 = stack->head->next;
                op3 = stack->head;
                rule = test_rule(count, op1, op2, op3);
        }
        else
                return SYNTAX_ERROR;

        if (!rule)/// rule == false
        {
                return SYNTAX_ERROR;
        }
        else
        {
                repeted_pop(stack, count + 1);
                ex_stack_push(stack, NON_TERM);
        }

        return SYNTAX_OK;
}

/**
 * Main function for working with expressions.
 */

int expression(MainData* data)
{
        int result;

	///initialization of stack and buffer
        stack_exp *stack = ex_stack_init();
        //inst_buffer *prog = init_inst_buffer();

	/// end of stack
        if (!ex_stack_push(stack, DOLLAR))
        {
                FREE_RESOURCES_RETURN(ERROR_INTERNAL)
        }

        //primitives *//op;
    //STsymPtr *symb;


        list_exp* top_stack_terminal;
        symbol_enum actual_symbol;

        bool success = false;

        do
        {
		/// control second and third token
            /// T_NONE is like NULL
                if(data->third_token->type != T_NONE)
                {
                        actual_symbol = get_symbol_from_token(data->third_token);
                        data->third_token->type = T_NONE;
                }
                else if(data->second_token->type != T_NONE)
                {
                        actual_symbol = get_symbol_from_token(data->second_token);
                        data->second_token->type = T_NONE;
                }
                else{
                    actual_symbol = get_symbol_from_token(data->token);
                }


                top_stack_terminal = stack_top_terminal(stack);


                if (top_stack_terminal == NULL)
                {
                        FREE_RESOURCES_RETURN(ERROR_INTERNAL)
                }

		/// use of precedence table
                switch (precedence_table[get_precedence_table_index(top_stack_terminal->symbol)][get_precedence_table_index(actual_symbol)])
                {
                	///shift
                        case S:
                                if (!insert_after_top_terminal(stack, STOP))
                                {
                                        FREE_RESOURCES_RETURN(ERROR_INTERNAL)
                                }

				/// append_op
                                switch(actual_symbol){
                                        case PLUS:
                                                //op = new_operator(ADD);
                                                
                                            //(pro
                                            // g, //op);
                                        //break;

                                        case MINUS:
                                                //op = new_operator(SUB);
                                        //(prog, //op);
                                        //break;

                                        case MUL_:
                                                //op = new_operator(MUL);
                                        //(prog, //op);
                                       // break;

                                        case DIV_:
                                                //op = new_operator(DIV);
                                        //(prog, //op);
                                       // break;

                                        case EQ_:
                                                //op = new_operator(EQ);
                                        //(prog, //op);
                                        //break;

                                        case NEQ:
                                                //op = new_operator(NOT);
                                        //(prog, //op);
                                        //break;

                                        case LEQ:
                                                //op = new_operator(LT_OR_EQ);
                                        //(prog, //op);
                                       // break;

                                        case LESS:
                                                //op = new_operator(LT);
                                        //(prog, //op);
                                        //break;

                                        case MEQ:
                                                //op = new_operator(GT_OR_EQ);
                                        //(prog, //op);
                                        //break;

                                        case MORE:
                                                //op = new_operator(GT);
                                        //(prog, //op);
                                       // break;

                                        case LEFT_BRACKET_:
                                                //op = new_operator(LEFT_BRACKET);
                                        //(prog, //op);
                                        //break;

                                        case RIGHT_BRACKET_:
                                                //op = new_operator(RIGHT_BRACKET);
                                        //(prog, //op);
                                        break;
                                        
					///identificator
                                        case ID:
//                                            if(data->third_token->type != T_NONE)
//                                            {
//                                                data->table->sym = symtable_lookup_add(&data->table, data->third_token->data->string);
//                                            }
//                                            else if(data->second_token->type != T_NONE)
//                                            {
//                                                data->table->sym = symtable_lookup_add(&data->table, data->second_token->data->string);
//                                            }
//                                            else
//                                            {
//                                                data->table->sym = symtable_lookup_add(&data->table, data->token->data->string);
//                                            }
//
//                                        //op = new_operand(data->table.sym);
//                                        //(prog, //op);
                                        break;
                                        
                                        /// integer
                                        case INT_:

//                                                symb = new_symbol();
//                                        &symb->type = INT;
//
//		                                if(data->third_token->type != T_NONE)
//		                                {
//		                                        symb->data->integer = data->third_token.data->u_int;
//		                                }
//		                                else if(data->second_token->type != T_NONE)
//		                                {
//		                                        symb->data->integer = data->second_token->data->u_int;
//		                                }
//		                                else
//		                                {
//		                                        symb->data->integer = data->token->data->u_int;
//		                                }
//
//                                        //op = new_operand(symb);
//                                        //(prog, //op);
                                        break;
                                        
                                        ///float
                                        case FLOAT_:

//                                                symb = new_symbol();
//                                        symb->type = FLOAT;
//
//		                                if(data->third_token->type != T_NONE)
//		                                {
//		                                        symb->data->flp = data->third_token.data->u_double;
//		                                }
//		                                else if(data->second_token->type != T_NONE)
//		                                {
//		                                        symb->data->flp = data->second_token->data->u_double;
//		                                }
//		                                else
//		                                {
//		                                        symb->data->flp = data->token->data->u_double;
//		                                }
//
//                                        //op = new_operand(symb);
//                                        //(prog, //op);
                                        break;
                                        
                                        //string
                                        case STRING_:

//                                            symb = new_symbol();
//                                            symb->type = STRING;
//
//		                                if(data->third_token->type != T_NONE)
//		                                {
//		                                        symb->data->string = data->third_token.data->string;
//		                                }
//		                                else if(data->second_token->type != T_NONE)
//		                                {
//		                                        symb->data->string = data->second_token.data->string;
//		                                }
//		                                else
//		                                {
//		                                        symb->data->string = data->token->data->string;
//		                                }
//
//                                            //op = new_operand(symb);
//                                            //(prog, //op);
                                            break;

                                        default:
                                                break;

                                }



			/// if push stack error
                        if(!ex_stack_push(stack, actual_symbol))
                        {
                                FREE_RESOURCES_RETURN(ERROR_INTERNAL)
                        }

			/// empty second and thirh token
                        if(data->third_token->type == T_NONE && data->second_token->type == T_NONE)
                        {
                            if ((result = getParserToken(data->token)))
                            {
                                    FREE_RESOURCES_RETURN(result)
                            }

                        }
                        break;

			/// equal
                        case Q:
                                ex_stack_push(stack, actual_symbol);
                                
				// append_op
                        switch(actual_symbol){
                                case PLUS:
                                        //op = new_operator(ADD);
                                //(prog, //op);
                                //break;

                                case MINUS:
                                        //op = new_operator(SUB);
                                //(prog, //op);
                                //break;

                                case MUL_:
                                        //op = new_operator(MUL);
                                //(prog, //op);
                               // break;

                                case DIV_:
                                        //op = new_operator(DIV);
                                //(prog, //op);
                               // break;

                                case EQ_:
                                        //op = new_operator(EQ);
                                //(prog, //op);
                               // break;

                                case NEQ:
                                        //op = new_operator(NOT);
                                //(prog, //op);
                                //break;

                                case LEQ:
                                        //op = new_operator(LT_OR_EQ);
                                //(prog, //op);
                               //break;

                                case LESS:
                                        //op = new_operator(LT);
                                //(prog, //op);
                                //break;

                                case MEQ:
                                        //op = new_operator(GT_OR_EQ);
                                //(prog, //op);
                                //break;

                                case MORE:
                                        //op = new_operator(GT);
                                //(prog, //op);
                                //break;

                                case LEFT_BRACKET_:
                                        //op = new_operator(LEFT_BRACKET);
                                //(prog, //op);
                                //break;

                                case RIGHT_BRACKET_:
                                        //op = new_operator(RIGHT_BRACKET);
                                //(prog, //op);
                                break;
                                
				/// identificator
                                case ID:

//                                        if(data->third_token->type != T_NONE)
//                                        {
//                                                data->table->sym = symtable_lookup_add(&data->table, data->third_token->data->string);
//                                        }
//                                        else if(data->second_token->type != T_NONE)
//                                        {
//                                                data->table->sym = symtable_lookup_add(&data->table, data->second_token->data->string);
//                                        }
//                                        else
//                                        {
//                                                data->table->sym = symtable_lookup_add(&data->table, data->token->data->string);
//                                        }
//
//                                //op = new_operand(data->table.sym);
//                                //(prog, //op);
                                break;
                                
				/// integer
                                case INT_:

//                                        symb = new_symbol();
//                                symb->type = INT;
//
//		                        if(data->third_token->type != T_NIL)
//		                        {
//		                                symb->data->integer = data->third_token->data->u_int;
//		                        }
//		                        else if(data->second_token->type != T_NIL)
//		                        {
//		                                symb->data->integer = data->second_token->data->u_int;
//		                        }
//		                        else
//		                        {
//		                                symb->data->integer = data->token->data->u_int;
//		                        }
//
//                                //op = new_operand(symb);
//                                //(prog, //op);
                                break;

				///float
                                case FLOAT_:

//                                        symb = new_symbol();
//                                symb->type = FLOAT;
//
//		                        if(data->third_token->type != T_NONE)
//		                        {
//		                                symb->data->flp = data->third_token->data->u_double;
//		                        }
//		                        else if(data->second_token->type != T_NONE)
//		                        {
//		                                symb->data->flp = data->second_token->data->u_double;
//		                        }
//		                        else
//		                        {
//		                                symb->data->flp = data->token->data->u_double;
//		                        }
//
//                                //op = new_operand(symb);
//                                //(prog, //op);
                                break;
                                
				///string
                                case STRING_:

//                                        symb = new_symbol();
//                                symb->type = STRING;
//
//		                        if(data->third_token->type != T_NIL)
//		                        {
//		                                symb->data->string = data->third_token->data->string;
//		                        }
//		                        else if(data->second_token->type != T_NIL)
//		                        {
//		                                symb->data->string = data->second_token->data->string;
//		                        }
//		                        else
//		                        {
//		                                symb->data->string = data->token->data->string;
//		                        }
//
//                                //op = new_operand(symb);
//                                //(prog, //op);
                                break;

                                default:
                                        break;

                        }
                        
			/// empty second and thirh token
                        if(data->third_token->type == T_NONE && data->second_token->type == T_NONE)
                        {
                            if ((result = getParserToken(data->token))){
                                FREE_RESOURCES_RETURN(result)
                            }

                        }
                        break;

			/// reduce
                        case R:
                                if ((result = reduce( stack)))
                                {
                                        FREE_RESOURCES_RETURN(result)
                                }

                        break;

			/// error
                        case E:
                        	// OK end
                                if (actual_symbol == DOLLAR && top_stack_terminal->symbol == DOLLAR)
                                        success = true;
                                else
                                {
                                        FREE_RESOURCES_RETURN(SYNTAX_ERROR)
                                }

                        break;

                        default:
                                break;
                }
        } while (!success);


	/// no end of stack
        if (ex_stack_empty(stack) == true)
        {
                FREE_RESOURCES_RETURN(ERROR_INTERNAL)
        }
        
	/// not reduced symbols
        list_exp* final_non_terminal = ex_stack_top(stack);

        if (final_non_terminal->symbol != NON_TERM)
        {
                FREE_RESOURCES_RETURN(SYNTAX_ERROR)
        }

        FREE_RESOURCES_RETURN(SYNTAX_OK)
}
