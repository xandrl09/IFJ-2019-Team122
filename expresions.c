/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */


#include "expresions.h"
#include "main.h"

expression_stack  e_stack;

int precedence_table[7][7] =
        {
               ///r |+- |*/ | ( | ) | i | $ |
                { E , S , S , S , R , S , R }, /// r (realtion operators) ==!= <= < >= >
                { R , R , S , S , R , S , R }, /// +-
                { R , R , R , S , R , S , R }, /// */ //
                { S , S , S , S , Q , S , E }, /// (
                { R , R , R , E , R , E , R }, /// )
                { R , R , R , E , R , E , R }, /// i (id, int, double, string)
                { S , S , S , S , E , S , E }  /// $
        };



/**
 *
 * @param num
 * @param op1
 * @param op2
 * @param op3
 * @return
 */
bool  process_rules(int num, expression_list *op1, expression_list *op2, expression_list *op3)
{
    if(num == 1)
    {
        if (op1->symbol == FLOAT_|| op1->symbol == INT_ || op1->symbol == ID  || op1->symbol == STRING_)
        {
            return true;
        }
        return false;
    }
    else if(num == 3)
    {
        if (op1->symbol == LEFT_BRACK_ && op2->symbol == NON_TERM && op3->symbol == RIGHT_BRACK_)
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
                default:
                    return false;/// bad operator
            }
        }
        return false;
    }
    else
    {
        return false;
    }
}


/**
 *
 * @param exp_stack
 * @return
 */
static int reduce( expression_stack *exp_stack)
{
        expression_list* op1 = NULL; expression_list* op2 = NULL; expression_list* op3 = NULL;
        bool rule;

        int count = 0;
        expression_list* temp = expression_stack_top(&e_stack);

    while (true)
    {
        if (temp->symbol == STOP)
        {
            break;
        }
        else
        {
            count++;
        }
        temp = temp->next;
    }


        if ( count == 1)
        {
                op1 = exp_stack->top;
                rule = process_rules(count, op1, NULL, NULL);
        }
        else if ( count == 3 )
        {
                op1 = exp_stack->top->next->next; op2 = exp_stack->top->next; op3 = exp_stack->top;
                rule = process_rules(count, op1, op2, op3);
        }
        else
        {
            expression_stack_destroy(exp_stack);
            errSyn();
            return SYNTAX_ERROR;
        }

        if (!rule)/// rule == false
        {
            expression_stack_destroy(exp_stack);
            errSyn();
            return SYNTAX_ERROR;
        }
        else
        {
            for (int i = 0; i < count + 1; i++)
            {
                expression_stack_pop(exp_stack);
            }
            expression_stack_push(exp_stack, NON_TERM);
        }

        return 0;
}


/**
 *
 * @param symbol
 * @return
 */
static index_enum table_index(symbol_enum symbol)
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

        case LEFT_BRACK_:
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
            if(symbol == RIGHT_BRACK_)
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
 *
 * @param token
 * @return
 */
static symbol_enum token_symb(T_token *token)
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
        case T_SUB:
            if(token->type == T_ADD)
            {
                return MINUS;
            }
            else
            {
                return PLUS;
            }
        case T_MUL:
            return MUL_;
        case T_DIV:
            return DIV_;
        case T_WH_N_DIV:
            return DIV_WH;
        case T_LBRACK:
            return LEFT_BRACK_;
        case T_RBRACK:
            return RIGHT_BRACK_;
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
 *
 * @param data
 * @return
 */
int expression(MainData* data)
{
	///initialization of stack and buffer
        expression_stack_init(&e_stack);

	/// end of stack
        expression_stack_push(&e_stack, DOLLAR);

        expression_list* top_terminal;
        symbol_enum actual_token;

        bool ending = false;

        do
        {
		/// control second and third token
		/// control is because
            /// T_NONE is like NULL
                if(data->third_token.type != T_NONE)
                {
                        actual_token = token_symb(&data->third_token);
                        data->third_token.type = T_NONE;
                }
                else if(data->second_token.type != T_NONE)
                {
                        actual_token = token_symb(&data->second_token);
                        data->second_token.type = T_NONE;
                }
                else
                {
                    actual_token = token_symb(&data->token);
                }


                top_terminal = stack_top_terminal(&e_stack);


		/// use of precedence table
                switch (precedence_table[table_index(top_terminal->symbol)][table_index(actual_token)])
                {
                    /// error
                    case E:
                        if (actual_token == DOLLAR && top_terminal->symbol == DOLLAR)
                        {
                            ending = true;/// OK end
                        }
                        else
                        {
                            expression_stack_destroy(&e_stack);
                            errSyn();
                        }
                        break;

                        /// reduce
                    case R:
                        reduce( &e_stack);
                        break;

			/// equal
                        case Q:
                            expression_stack_push(&e_stack, actual_token);

                        /// empty second and thirh token
                        if(data->third_token.type == T_NONE && data->second_token.type == T_NONE)
                        {
                            getParserToken(&data->token);
                        }
                        break;

                        ///shift
                    case S:
                        insert_after_top_terminal (&e_stack, STOP);
                        expression_stack_push(&e_stack, actual_token);/// push stack
                        /// empty second and thirh token
                        if(data->third_token.type == T_NONE && data->second_token.type == T_NONE)
                        {
                            getParserToken(&data->token);
                        }
                        break;
                        default:
                                break;
                }
        } while (!ending);

        
	/// not reduced symbols
        expression_list* last = expression_stack_top(&e_stack);

        if (last->symbol != NON_TERM)
        {
            errSyn();
        }
        expression_stack_destroy(&e_stack);
        return SYNTAX_OK;
}
