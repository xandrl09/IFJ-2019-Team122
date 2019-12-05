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


/**
 * Precedence table, more info in documentation
 */
int precedence_table[7][7] =
        {
               ///r |+- |*/ | ( | ) | i | $ |
                { E , S , S , S , R , S , R }, /// r
                { R , R , S , S , R , S , R }, /// +-
                { R , R , R , S , R , S , R }, /// */ //
                { S , S , S , S , Q , S , E }, /// (
                { R , R , R , E , R , E , R }, /// )
                { R , R , R , E , R , E , R }, /// i
                { S , S , S , S , E , S , E }  /// $
        };

expression_stack  e_stack;


/**
 * Function check if rules are ok
 * @param count number of operands
 * @param operand1 first operand in processed rule
 * @param operand2 second operand in processed rule
 * @param operand3 third operand in processed rule
 * @return false if there is error
 */
bool  process_rules(int count, expression_list *operand1, expression_list *operand2, expression_list *operand3)
{
    if(count == 3)
    {
        if (operand1->symbol == LEFT_BRACK_ && operand2->symbol == NO_TERMINAL && operand3->symbol == RIGHT_BRACK_)
        {
            return true;
        }
        if (operand1->symbol == NO_TERMINAL && operand3->symbol == NO_TERMINAL)
        {
            switch (operand2->symbol)
            {
                case PLUS_:
                case MINUS_:
                case MUL_:
                case DIV_:
                case DIV_WH_:
                case EQ_:
                case N_EQ:
                case L_EQ:
                case LESS:
                case M_EQ:
                case MORE:
                    return true;
                default:
                    return false;
            }
        }
        return false;
    }
    else if(count == 1)
    {
        if (operand1->symbol == FLOAT_|| operand1->symbol == INT_ ||
                operand1->symbol == ID  || operand1->symbol == STRING_)
        {
            return true;
        }
        else
        {
            return false;
        }

    }
    else
    {
        return false;
    }
}


/**
 * Function reduces stack
 * @param exp_stack pointer to stack
 * @return
 */
static int reduce( expression_stack *exp_stack)
{
    expression_list* op1 = NULL; expression_list* op2 = NULL; expression_list* op3 = NULL;
    bool rule;

    int count = 0;
    expression_list* temp = expression_stack_top(&e_stack);

    /// number of values before stop
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

    if ( count == 3 )/// normal operation
    {
        op1 = exp_stack->top->next->next; op2 = exp_stack->top->next; op3 = exp_stack->top;
        rule = process_rules(count, op1, op2, op3);
    }
    else if ( count == 1)/// value into nonterminal
    {
        op1 = exp_stack->top;
        rule = process_rules(count, op1, NULL, NULL);
    }
    else /// invalid operation
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
        else /// popping stack
        {
            for (int i = 0; i < count + 1; i++)
            {
                expression_stack_pop(exp_stack);
            }
            expression_stack_push(exp_stack, NO_TERMINAL);
        }

        return 0;
}


/**
 * function turns expresion enum into tables rows
 * @param symbol
 * @return
 */
static index_enum table_index(symbol_enum symbol)
{
    switch (symbol)
    {
        case PLUS_:
            return T_PLUS;
        case MINUS_:
            return T_PLUS;

        case MUL_:
            return T_MUL_DI;
        case DIV_:
            return T_MUL_DI;
        case DIV_WH_:
            return T_MUL_DI;

        case EQ_:
            return T_REL;
        case N_EQ:
            return T_REL;
        case L_EQ:
            return T_REL;
        case LESS:
            return T_REL;
        case M_EQ:
            return T_REL;
        case MORE:
            return T_REL;

        case LEFT_BRACK_:
            return T_LEFT_B;

        case ID:
            return T_IDS;
        case INT_:
            return T_IDS;
        case FLOAT_:
            return T_IDS;
        case STRING_:
            return T_IDS;

        default:
            if(symbol == RIGHT_BRACK_)
            {
                return T_RIGHT_B;
            }
            else
            {
                return T_END;
            }
    }
}


/**
 * function turns teken data to expresion enum
 * @param token T_token from parser.c
 * @return
 */
static symbol_enum token_symb(T_token *token)
{
    switch (token->type)
    {
        case T_MORE:
            return MORE;
        case T_ADD:
        case T_SUB:
            if(token->type == T_ADD)
            {
                return MINUS_;
            }
            else
            {
                return PLUS_;
            }
        case T_MUL:
            return MUL_;
        case T_DIV:
            return DIV_;
        case T_WH_N_DIV:
            return DIV_WH_;
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
                case T_EQ_COMP:
                    return EQ_;
                case T_NEQ:
                    return N_EQ;
                case T_LESS_EQ:
                    return L_EQ;
                case T_LESS:
                    return LESS;
                case T_MORE_EQ:
                    return M_EQ;
                default:
                    return DOLLAR;
            }
    }
}


/**
 * Function process expressions for parser.c
 * @param data Main struct from parser.h
 * @return
 */
int expression(MainData* data)
{
	    ///initialization of stack
        expression_stack_init(&e_stack);

	    /// end of stack
        expression_stack_push(&e_stack, DOLLAR);

        expression_list* top_terminal;
        symbol_enum actual_token;

        bool ending = false;

        do
        {
		/// control second and third token
		/// control is because ID = in parser
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
                        /// empty second and thirth token
                        if(data->third_token.type == T_NONE && data->second_token.type == T_NONE)
                        {
                            getParserToken(&data->token);
                        }
                        break;
                        default:
                                break;
                }
        } while (!ending); /// works till ending == true

        

        expression_list* last = expression_stack_top(&e_stack);
        /// not reduced symbols in stack
        if (last->symbol != NO_TERMINAL)
        {
            errSyn();
        }
        expression_stack_destroy(&e_stack);
        return SYNTAX_OK;
}
