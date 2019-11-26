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
		expression_stack_destroy(stack);									\
		return _return;												\


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
                { R , R , R , S , R , S , R }, /// */ //
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
                case T_WH_N_DIV:
                        return DIV_WH;
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
                case MINUS:
                    return I_PLUS_MINUS;

                case MUL_:
                case DIV_:
                case DIV_WH:
                    return I_MUL_DIV;

                case EQ_:
                case NEQ:
                case LEQ:
                case LESS:
                case MEQ:
                case MORE:
                    return I_REL_OP;

                case LEFT_BRACKET_:
                        return I_LEFT_BRACKET;

                case ID:
                    return I_DATA;
                case INT_:
                    return I_DATA;
                case FLOAT_:
                    return I_DATA;
                case STRING_:
                    return I_DATA;

                default:
                   if(symbol == RIGHT_BRACKET_)
                   {
                            return I_RIGHT_BRACKET;
                   }
                   else
                   {
                       return I_DOLLAR;
                   }
        }
}


/**
 * Function function counts number of symbols after stop symbol on stack.
 *
 * @param stop_found Pointer to bool variable which will be changed to true if stop was found else to false.
 * @return Number of characters after stop symbol. Is valid only when stop_found was set to true.
 */
static int num_of_symbols_before_stop(bool *stop_found, expression_stack *stack)
{
        expression_list* temp = expression_stack_top(stack);
        int count = 0;

        while (true)
        {
            if (temp->symbol != STOP)
            {
                *stop_found = false;
                count++;
            }
            else{
                *stop_found = true;
                break;
            }
            temp = temp->next;
        }
        return count;
}


/**
 * Function tests if symbols in parameters are valid according to rules.
 *
 * @param num Number of valid symbols in parameter.
 * @return NOT_A_RULE if no rule is found or returns rule which is valid.
 */
bool  test_rule(int num, expression_list* op1, expression_list* op2, expression_list* op3)
{
        switch (num)
        {
                case 1:
                        /// rule E -> i
                        if (op1->symbol == ID || op1->symbol == INT_ || op1->symbol == FLOAT_ || op1->symbol == STRING_)
                        {
                            return true;
                        }
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
static int reduce( expression_stack *stack)
{

        expression_list* op1 = NULL;
        expression_list* op2 = NULL;
        expression_list* op3 = NULL;

        bool rule;
        bool found = false;

        int count = num_of_symbols_before_stop(&found, stack);


        if (count == 1 && found)
        {
                op1 = stack->top;
                rule = test_rule(count, op1, NULL, NULL);
        }
        else if (count == 3 && found)
        {
                op1 = stack->top->next->next;
                op2 = stack->top->next;
                op3 = stack->top;
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
            for (int i = 0; i < count + 1; i++)
            {
                expression_stack_pop(stack);
            }
            expression_stack_push(stack, NON_TERM);
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
        expression_stack *stack = expression_stack_init();

	/// end of stack
        expression_stack_push(stack, DOLLAR);


        expression_list* top_stack_terminal;
        symbol_enum actual_symbol;

        bool success = false;

        do
        {
		/// control second and third token
		/// control is because
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
                else
                {
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
                                insert_after_top_terminal(stack, STOP);

                                switch(actual_symbol){
                                        case ID:///identificator
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
                                        break;

                                        case INT_:/// integer
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
                                        break;

                                        case FLOAT_:///float

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
                                        break;

                                        case STRING_:///string
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
                                        break;
                                        default:
                                            break;
                                }

                        expression_stack_push(stack, actual_symbol);/// push stack

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
                            expression_stack_push(stack, actual_symbol);

                        /// empty second and thirh token
                        if(data->third_token->type == T_NONE && data->second_token->type == T_NONE)
                        {
                            if ((result = getParserToken(data->token)))
                            {
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
                                if (actual_symbol == DOLLAR && top_stack_terminal->symbol == DOLLAR)
                                {
                                    success = true;/// OK end
                                }
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
        if (expression_stack_empty(stack) == true)
        {
            FREE_RESOURCES_RETURN(ERROR_INTERNAL)
        }
        
	/// not reduced symbols
        expression_list* final_non_terminal = expression_stack_top(stack);

        if (final_non_terminal->symbol != NON_TERM)
        {
            FREE_RESOURCES_RETURN(SYNTAX_ERROR)
        }

    FREE_RESOURCES_RETURN(SYNTAX_OK)
}
