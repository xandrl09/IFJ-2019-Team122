/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

/** STANDARDS
 * Starting labels are marked with &
 * Closing labels are marked with *
 * Function arguments are marked with % and appropriate number, eg. %1, %2 etc.0
 * Return value is always called _retval
 * Flow control labels are marked with [&,*]_$, eg. &_$IF0, &_$WHILE32
 * &_$bool1 - used in gen_expression_to_bool, only one instance
 * &_$nil1 - used in gen_expression_to_bool, only one instance
 * $op1, $op2, $op1type, $op2type
 */

// This queue of tokens is taken directly from scanner
extern tDLList *tokenQueue;

// Stack used to track lines of IFJcode19 that generator cannot print right away
extern CDStack *code_gen_stack;

TStack postfix_expr;

// This queue of tokens is taken from global symtable
tDLList *global_var_scope;
// This queue of toknes is taken from local symtable (changes quite often)
tDLList *local_var_scope;

int while_lbl_id = 0;

int bool_lbl_id = 0;
int nil_lbl_id = 0;
int typecheck_jumps_id = 0;
int else_lbl_id = 0;

int print_lbl_id = 0;

int scope_nesting_lvl = 0;
char current_func_lbl[255] = "";


void handle_inputs_line()   {
    printf("CREATEFRAME\n");
    printf("JUMP *$inputs\n");
    printf("LABEL &$inputs\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@_retval\n");
    printf("MOVE LF@_retval nil@nil\n");
    printf("DEFVAR LF@$inputs\n");
    printf("READ LF@$inputs string\n");
    printf("MOVE LF@_retval LF@inputs\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL *$inputs\n");
    printf("CALL &$inputs");
    printf("MOVE LF@_retval TF@_retval\n");
    printf("MOVE GF@_result TF@_retval\n");
    if (strcmp(tokenQueue->First->rptr->rptr->value, "inputs" ) == 0)
        printf("MOVE LF@%s GF@_result\n", tokenQueue->First->value);
}

void handle_inputf_line()   {
    printf("CREATEFRAME\n");
    printf("JUMP *$inputf\n");
    printf("LABEL &$inputf\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@_retval\n");
    printf("MOVE LF@_retval nil@nil\n");
    printf("DEFVAR LF@$inputf\n");
    printf("READ LF@$inputf float\n");
    printf("MOVE LF@_retval LF@$inputf\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL *$inputf\n");
    printf("CALL &$inputf\n");
    printf("MOVE LF@_retval TF@_retval\n");
    printf("MOVE GF@_result TF@_retval\n");
    if (strcmp(tokenQueue->First->rptr->rptr->value, "inputf" ) == 0)
        printf("MOVE LF@%s GF@_result\n", tokenQueue->First->value);
}

void handle_inputi_line()   {
    printf("CREATEFRAME\n");
    printf("JUMP *$inputi\n");
    printf("LABEL &$inputi\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@_retval\n");
    printf("MOVE LF@_retval nil@nil\n");
    printf("DEFVAR LF@$inputi\n");
    printf("READ LF@$inputi int\n");
    printf("MOVE LF@_retval LF@$inputi\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL *$inputi\n");
    printf("CALL &$inputi\n");
    printf("MOVE LF@_retval TF@_retval\n");
    printf("MOVE GF@_result TF@_retval\n");
    // now - should I assign it?
    if (strcmp(tokenQueue->First->rptr->rptr->value, "inputi" ) == 0)
        printf("MOVE LF@%s GF@_result\n", tokenQueue->First->value);
}


/**
 * @brief  Generates code needed to handle lines like these
 * my_variable = foo(arg1, arg)
 */
void handle_function_call_with_assignment() {
    // first we need to call function on the right side
    // in order to do so we need to set tokenQueue->Act to function's identifier
    if(scope_nesting_lvl <= 0 && !is_duplicate_definition(tokenQueue->First->value, global_var_scope)) {
        DLInsertLast(global_var_scope, identifier, tokenQueue->First->value, -1);
    }
    else if (scope_nesting_lvl > 0 && !is_duplicate_definition(tokenQueue->First->value, local_var_scope))     {
        DLInsertLast(local_var_scope, identifier, tokenQueue->First->value, -1);
    }
    DLFirst(tokenQueue);
    DLSucc(tokenQueue);
    DLSucc(tokenQueue);
    handle_function_call();
    printf("MOVE GF@_result TF@_retval\n");
    printf("MOVE %s@%s GF@_result\n", get_variable_scope(tokenQueue->First->value), tokenQueue->First->value);
    // then we need to assign function's return value to variable on the right side
}


/**
 * @brief Generates code necessary to assign bool value to expression's result
 */
void gen_expression_to_bool()   {
    char bool_lbl[MAX_TOKEN_LEN] = "";
    char nil_lbl[MAX_TOKEN_LEN] = "";
    sprintf(bool_lbl, "&_$bool%i", bool_lbl_id++);
    sprintf(nil_lbl, "&_$nil%i", nil_lbl_id++);

    printf("TYPE GF@$op1type GF@_result\n");
    printf("JUMPIFEQ %s GF@$op1type string@bool\n", bool_lbl);
    printf("JUMPIFEQ %s GF@$op1type string@nil\n", nil_lbl);

    // in case it's neither bool or nil value
    // TODO - low prio - hodnota ve vyrazu bude 0 a priradim true!
    printf("MOVE GF@_result bool@true\n"); // assign true value
    printf("JUMP %s\n", bool_lbl); // now we're ready to jump to the end of type-check
    printf("LABEL %s\n", nil_lbl); // in case the value in expression is nil
    printf("MOVE GF@_result bool@false\n");
    printf("JUMP %s\n", bool_lbl);  // type-check end here with LABEL &_$bool42
    printf("LABEL %s\n", bool_lbl);
}

/**
 * @brief Prints opening label and pushes closing label to code_gen_stack
 */
void gen_while_labels() {
    printf("LABEL &_$WHILE%i\n", while_lbl_id);
    char end_of_while_cycle [MAX_TOKEN_LEN] = "";
    sprintf(end_of_while_cycle, "JUMP &_$WHILE%i\nLABEL *_$WHILE%i\n", while_lbl_id, while_lbl_id);
    CDpush(code_gen_stack, end_of_while_cycle);
}

void handle_eof()   {
    printf("CALL _end\n");
    printf("LABEL &_$INIT\n");
    generate_variables_from_queue(global_var_scope, "TF");
    printf("CALL &_main\n");
    printf("LABEL _end\n");
}

// while a > 3:
void handle_while()    {
    gen_while_labels();
    tDLList expression = get_expression_queue(1);
    postfix_expr = infix_2_postfix(&expression);
    printf("CLEARS\n");
    generate_expression();
    gen_expression_to_bool(); // this turns our expression's result to either true or false
    printf("JUMPIFNEQ *_$WHILE%i GF@_result bool@true\n", while_lbl_id);
    while_lbl_id++;
}

/**
 * @brief This function takes global tokenQueue and copies its content into expression_queueE
 * EXCEPT for the first 'to_skip' tokens
 * @param to_skip Number of toknes to not-include in the expression, starting from first token in queue
 * @return 5 + 4 * y
 */
tDLList get_expression_queue(int to_skip) {
    tDLList *expression_queue = malloc(sizeof(tDLList));
    DLInitList(expression_queue);
    for(int i = 0; i < to_skip; i++)   { DLSucc(tokenQueue); }
    while(tokenQueue->Act != NULL && tokenQueue->Act->rptr != NULL &&
          tokenQueue->Act->rptr->type != EoF && tokenQueue->Act->type != specialChar)   {
        DLInsertLast(expression_queue, tokenQueue->Act->type, tokenQueue->Act->value, -1);
        DLSucc(tokenQueue);
    }
    DLFirst(expression_queue);
    return *expression_queue;
}

/**
 * if arg1 < arg2+4+38:
 * if z:
 */
void handle_if() {
    DLFirst(tokenQueue);
    tDLList expression;
    int is_relational = is_relational_op(tokenQueue->First->rptr->rptr->value);
    expression = get_expression_queue(1);
    postfix_expr = infix_2_postfix(&expression);
    printf("CLEARS\n");
    generate_expression();
    if (is_relational) {
        gen_relational_comparison(tokenQueue->First->rptr->rptr->value);
        gen_expression_to_bool();
    }
    printf("JUMPIFNEQ &else%i GF@_result bool@true\n", else_lbl_id);
}

int is_relational_op(char* val) {
    char* relational_ops[6] = {"==", "<=", ">=", "!=", ">", "<"};
    for(int i = 0; i < 6; i++)  {
        if(strcmp(relational_ops[i], val) == 0)
            return 1;
    }
    return 0;
}

void handle_else()  {
    printf("JUMP *else%i\n", else_lbl_id);
    printf("LABEL &else%i\n", else_lbl_id);
    char tmp[MAX_LINE_LENGTH] = "";
    sprintf(tmp, "LABEL *else%i\n", else_lbl_id++);
    CDpush(code_gen_stack, tmp);
}



/**
 * We're assuming tokenQueue->Act points at the identifier on left-hand side
 * a = 4 + d * 3
 */
void handle_assignment()    {
    DLFirst(tokenQueue);
    if(scope_nesting_lvl <= 0 && !is_duplicate_definition(tokenQueue->First->value, global_var_scope)) {
        DLInsertLast(global_var_scope, identifier, tokenQueue->First->value, -1);
    }
    else if (scope_nesting_lvl > 0 && !is_duplicate_definition(tokenQueue->First->value, local_var_scope))     {
        DLInsertLast(local_var_scope, identifier, tokenQueue->First->value, -1);
    }
    tDLList expression = get_expression_queue(2);
    if (tokenQueue->First->rptr->rptr->rptr->type == Operator)
        postfix_expr = infix_2_postfix(&expression);
    else
        Tpush(&postfix_expr, *tokenQueue->First->rptr->rptr);

    printf("CLEARS\n");
    generate_expression();
    char* scope = get_variable_scope(tokenQueue->First->value);
    printf("MOVE %s@%s GF@_result\n", scope, tokenQueue->First->value);
}


/**
 * When program meets the line return my_var, this gets called
 */
void handle_return()   {
    if (tokenQueue->First->rptr != NULL &&
        tokenQueue->First->rptr->type != EOL &&
        tokenQueue->First->rptr->type != EoF &&
        strcmp(tokenQueue->First->value, "return") == 0)  {

        char* scope = get_variable_scope(tokenQueue->First->rptr->value);
        printf("MOVE LF@_retval %s@%s\n", scope, tokenQueue->First->rptr->value);
    }

    printf("JUMP *%s_INIT\n", current_func_lbl);
    printf("LABEL &%s_INIT\n", current_func_lbl);
    //printf("CREATEFRAME\n");    //5:42 zmena - pokus o scope
    generate_variables_from_queue(local_var_scope, "TF");
    DLDisposeList(local_var_scope);
    DLInitList(local_var_scope);
    printf("JUMP &%s\n", current_func_lbl);
    printf("LABEL *%s_INIT\n", current_func_lbl);
    strcpy(current_func_lbl, "");
}

void handle_def() {
    DLFirst(tokenQueue);
    char *lbl = tokenQueue->First->rptr->value;
    strcpy(current_func_lbl, lbl);
    printf("JUMP *%s\n", lbl); // jump to the end of function
    printf("JUMP &%s_INIT\n", lbl);
    printf("LABEL &%s\n", lbl);

    // generate variable declaration from thw whole scope

    printf("PUSHFRAME\n");
    //generate_variable("_retval", "LF", local_var_scope); 5:49 - pokuus o scope
    printf("DEFVAR LF@_retval\n");
    printf("MOVE LF@_retval nil@nil\n");
    gen_function_def_args();

    // The function's code itself

    // pushes lines of code to be printed later
    char x [MAX_LINE_LENGTH]= "";
    sprintf(x, "POPFRAME\nRETURN\nLABEL *%s\n", lbl);

    CDpush(code_gen_stack, x);
}

void handle_dedent()   {
    printf("%s", CDtop(*code_gen_stack));
    CDpop(code_gen_stack);
}

/**
 * tokenQueue->Act MUST be set to the function's identifier
 */
void handle_function_call() {
    char* lbl = tokenQueue->Act->value;
    printf("CREATEFRAME\n");
    int argc = gen_function_call_args();
    if (strcmp(lbl, "print") == 0)
        gen_print_method(argc);
    else if (strcmp(lbl, "inputi") == 0)
        handle_inputi_line();
    else if (strcmp(lbl, "inputf") == 0)
        handle_inputf_line();
    else if (strcmp(lbl, "inputs") == 0)
        handle_inputs_line();
    else    {
        printf("CALL &%s_INIT\n", lbl);
        printf("MOVE LF@_retval TF@_retval\n");
    }
}

/**
 *
 */
void gen_print_method(int num_of_args) {
    printf("JUMP *$print%i_for_%i\n", print_lbl_id, num_of_args);
    printf("LABEL &$print%i_for_%i\n", print_lbl_id, num_of_args);
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@_retval\n");
    printf("MOVE LF@_retval nil@nil\n");
    for(int i = 1; i <= num_of_args; i++) {
        printf("WRITE LF@%%%i\n", i);
        printf("WRITE string@\\032\n");
    }
    printf("WRITE string@\\010\n"); // put end of line
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL *$print%i_for_%i\n", print_lbl_id, num_of_args);
    printf("CALL &$print%i_for_%i\n", print_lbl_id, num_of_args);
    print_lbl_id++;
}

char* get_type_and_edit_value(Token *token)    {
    switch(token->type) {
        case floatingPoint:
            sprintf(token->value, "%a", atof(token->value));
            return "float";
        case integer: return "int";
        case string:
        case docString:
            turn_whitespace_to_ascii(token);
            return "string";
        case identifier: return "LF";
        default: fprintf(stderr, "Encountered some strange token in generator, type: %i\n", token->type);
    }
}

void turn_whitespace_to_ascii(Token *token) {
    char temp [255] = "";
    strcpy(temp, token->value);
    int i = 251;
    for (i = 251; i >= 0; i--)  {
        if (temp[i] == ' ') {
            temp[i] = '\0';
            strcat(temp, "\\032");
            strcat(temp, token->value + i + 1);
        }
        else if (temp[i] == '\t')   {
            temp[i] = '\0';
            strcat(temp, "\\009");
            strcat(temp, token->value + i + 1);
        }
        else if (temp[i] == '\n')   {
            temp[i] = '\0';
            strcat(temp, "\\010");
            strcat(temp, token->value + i + 1);
        }
    }
    strcpy(token->value, temp);
}

/**
 * Generates both tokens and code for function's arguments at the start of a function
 * Function signature looks like this: def foo(arg1, arg2):
 */
void gen_function_def_args() {
    DLFirst(tokenQueue);
    tokenQueue->Act = tokenQueue->First->rptr->rptr->rptr; // first argument is now active
    int arg_cnt = 0;
    while(tokenQueue->Act != NULL && tokenQueue->Act->type != specialChar && tokenQueue->Act->type != EOL)  {
        printf("DEFVAR LF@%s\n", tokenQueue->Act->value);
        printf("MOVE LF@%s LF@%%%i\n", tokenQueue->Act->value, ++arg_cnt);
        //DLInsertLast(local_var_scope, nil, tokenQueue->Act->value, -1);
        DLSucc(tokenQueue);
        DLSucc(tokenQueue);
    }
}

/**
 *  my_function(argument1, argument22):
 *
 */
int gen_function_call_args()   {
    //DLFirst(tokenQueue);
    //int arg_is_string
    if(tokenQueue->Act->rptr != NULL)
        tokenQueue->Act = tokenQueue->Act->rptr->rptr;    // we're pointing at first argument
    else
        return 0;
    int arg_cnt = 0;
    // in case arguments are not string, eg foo(a, b, 42)
//    if (strcmp(tokenQueue->Act->value, "\"") == 0)
//        DLSucc(tokenQueue);
    while (tokenQueue->Act != NULL && tokenQueue->Act->type  != EOL && tokenQueue->Act->type != DEDENT) {
        if (tokenQueue->Act->type == specialChar)   {
            //      if (strcmp(tokenQueue->Act->value, "\"") == 0 || strcmp(tokenQueue->Act->value, "'") == 0)  {

            //    } else
            break;
        }
        printf("DEFVAR TF@%%%i\n", ++arg_cnt);
        printf("MOVE TF@%%%i %s@%s\n", arg_cnt, get_type_and_edit_value(tokenQueue->Act), tokenQueue->Act->value);
        DLSucc(tokenQueue);
        DLSucc(tokenQueue);
    }
    return arg_cnt;
}

int is_duplicate_definition(char *identifier, tDLList *queue) {
    DLFirst(queue);
    while(queue->Act != NULL)   {
        if (strcmp(identifier, queue->Act->value) == 0)
            return 1;
        DLSucc(queue);
    }
    return 0;
}

/**
 * @param token Token representing a variable
 * @param scope Either GF, TF or LF - based on IFJ19code's scope
 */
void gen_var_declaration_code(Token *token, char *scope) {
    char var_name[MAX_LINE_LENGTH] = "";
    sprintf(var_name, "%s@%s", scope, token->value);
    printf("DEFVAR %s\n", var_name);
    printf("MOVE %s nil@nil\n", var_name);
}

/**
 * @brief Takes line of tokens from scanner and cuts INDENT and DEDENT tokens
 * @param queue Queue of tokens that's supposed to be prepared
 */
void prepare_line_of_tokens(tDLList *queue) {
    DLFirst(queue);
    while(queue->Act->type == DEDENT || queue->Act->type == INDENT) {
        DLSucc(queue);
        free(queue->First);
        queue->First = queue->Act;
        queue->First->lptr = NULL;
    }
    DLFirst(queue);
}


/**
 * @brief This function is called for global variables and then for each function definition
 * @param queue Queue of tokens from symtable for current scope
 * @param scope Current scope (GF, TF, LF)
 */
void generate_variables_from_queue(tDLList *queue, char *scope) {
    DLFirst(queue);
    //char defined[255];
    while(queue->Act != NULL)    {
        gen_var_declaration_code(queue->Act, scope);
        DLSucc(queue);
    }
}

/**
 * @brief Generates both text and token for a given variable
 * @param name Variable's name
 * @param scope Variable's scope (GF/TF/LF)
 * @param queue Variable's queue
 */
void generate_variable(char *name, char *scope, tDLList *queue) {
    DLInsertLast(queue, nil, name, -1);
    DLLast(queue);
    gen_var_declaration_code(queue->Last, scope);
    DLFirst(queue);
}


// TODO - mozna ze to fungje?
char* get_variable_scope(char* identifier)  {
    return "LF";
}


/**
 * @brief This function generates code from CLEARS to POPS GF@_result, including typecheck
 * @param active_token First token in the expression, it's a token from tokenQueue
 * Ends with
 *  LABEL $type_check_passed
 */
void generate_expression() {
    int i = 0;
//    printf("CLEARS\n");
    Token token;
    while (postfix_expr.arr[i] != NULL) {
        token = *postfix_expr.arr[i];
        if (token.type == Operator) {
            gen_switch_operands();
            gen_assign_types();
            gen_typecheck_jumps();
            gen_actual_computation(token.value);
            typecheck_jumps_id++;
        } else if (token.type == identifier) {
            char *scope = get_variable_scope(token.value);
            printf("PUSHS %s@%s\n", scope, token.value);
        } else { // token is literal
            char *type = get_type_and_edit_value(&token);
            printf("PUSHS %s@%s\n", type, token.value);
        }
        i++;
    }
    printf("POPS GF@_result\n");
}

void gen_actual_computation(char* operator) {
    gen_op1_is_float();
    gen_op1_is_int();
    gen_op1_is_string();
    // now we need to check the second operator
    gen_ops_are_int(operator);
    gen_ops_are_float(operator);
    gen_ops_are_string(operator);

    gen_int_float(operator);
    gen_float_int(operator);
    printf("LABEL $type_check_passed%i\n", typecheck_jumps_id);
}

void gen_relational_comparison(char* op)    {
    char* instr = "";
    if (strcmp(op, "<=") == 0)
        instr = "LTS";
    else if (strcmp(op, ">=") == 0)
        instr = "GTS";
    else
        return;
    printf("POP GF@$op2\n");
    printf("POP GF@$op1\n");
    printf("PUSHS GF@$op1\n");
    printf("PUSHS GF@$op2\n");
    printf("PUSHS GF@$op1\n");
    printf("PUSHS GF@$op2\n");
    printf("%s\n", instr);
    printf("POPS GF@$op1\n");
    printf("EQS\n");
    printf("PUSHS GF@$op1\n");
    printf("ORS\n");
    printf("POPS GF@_result\n");
}

void gen_float_int(char* operator)  {
    printf("LABEL $float_int%i\n", typecheck_jumps_id);
    printf("PUSHS GF@$op2\n");
    printf("INT2FLOATS\n");
    gen_expression_operation(operator);
    printf("JUMP $type_check_passed%i\n", typecheck_jumps_id);
}

void gen_int_float(char* operator) {
    printf("LABEL $int_float%i\n", typecheck_jumps_id);
    printf("INT2FLOATS\n");
    printf("PUSHS GF@$op2\n");
    gen_expression_operation(operator);
    printf("JUMP $type_check_passed%i\n", typecheck_jumps_id);
}


void gen_ops_are_string(char *operator_value)   {
    printf("LABEL $ops_are_string%i\n", typecheck_jumps_id);
    printf("POPS GF@$op1\n");
    if (strcmp(operator_value, "+") == 0)
        printf("CONCAT GF@_result GF@$op1 GF@$op2\n");
    else
        printf("EXIT int@4\n");
    printf("PUSHS GF@_result\n");
    printf("JUMP $type_check_passed%i\n", typecheck_jumps_id);
}

void gen_ops_are_float(char *operator_value) {
    printf("LABEL $ops_are_float%i\n", typecheck_jumps_id);
    printf("PUSHS GF@$op2\n");
    gen_expression_operation(operator_value);
    printf("JUMP $type_check_passed%i\n", typecheck_jumps_id);
}

void gen_op1_is_string()    {
    printf("LABEL $op1_is_string%i\n", typecheck_jumps_id);
    printf("JUMPIFEQ $ops_are_string%i GF@$op2type string@string\n", typecheck_jumps_id);
    printf("EXIT int@4\n");
}

void gen_op1_is_int()  {
    printf("LABEL $op1_is_int%i\n", typecheck_jumps_id);
    printf("JUMPIFEQ $ops_are_int%i GF@$op2type string@int\n", typecheck_jumps_id);
    printf("JUMPIFEQ $int_float%i GF@$op2type string@float\n", typecheck_jumps_id);
    printf("EXIT int@4\n"); // in case op1 is int and op2 is string
}

void gen_ops_are_int(char *operator_value) {
    printf("LABEL $ops_are_int%i\n", typecheck_jumps_id);
    printf("PUSHS GF@$op2\n");
    gen_expression_operation(operator_value);
    printf("JUMP $type_check_passed%i\n", typecheck_jumps_id);
}

void gen_expression_operation(char *operator_value) {
    if (strcmp(operator_value, "+") == 0)
        printf("ADDS\n");
    else if (strcmp(operator_value, "-") == 0)
        printf("SUBS\n");
    else if (strcmp(operator_value, "*") == 0)
        printf("MULS\n");
    else if (strcmp(operator_value, "/") == 0)
        printf("DIVS\n");
    else if (strcmp(operator_value, "//") == 0)
        printf("IDIVS\n");
    else if (strcmp(operator_value, "<") == 0)
        printf("LTS\n");
    else if (strcmp(operator_value, "==") == 0)
        printf("EQS\n");
    else if (strcmp(operator_value, ">") == 0)
        printf("GTS\n");
    else return;
}

void gen_op1_is_float() {
    printf("LABEL $op1_is_float%i\n", typecheck_jumps_id);
    printf("JUMPIFEQ $float_int%i GF@$op2type string@int\n", typecheck_jumps_id);
    printf("JUMPIFEQ $ops_are_float%i GF@$op2type string@float\n", typecheck_jumps_id);
    printf("EXIT int@4\n"); // in case op1 is int and op2 is string
}

/*
 * Target output:
   JUMPIFEQ op1_is_int42 GF@op1type string@int
   JUMPIFEQ op1_is_float42 GF@op1type string@float
   JUMPIFEQ op1_is_string42 GF@optype1 string@string
   EXIT int@4
 */
void gen_typecheck_jumps()  {
    printf("JUMPIFEQ $op1_is_int%i GF@$op1type string@int\n", typecheck_jumps_id);
    printf("JUMPIFEQ $op1_is_float%i GF@$op1type string@float\n", typecheck_jumps_id);
    printf("JUMPIFEQ $op1_is_string%i GF@$op1type string@string\n", typecheck_jumps_id);
    printf("EXIT int@4\n");
}

// generates the same instructions every time
void gen_assign_types() {
    printf("TYPE GF@$op1type GF@$op1\n");
    printf("TYPE GF@$op2type GF@$op2\n");
}

// generates the same instructions every time
void gen_switch_operands()  {
    printf("POPS GF@$op2\n");
    printf("POPS GF@$op1\n");
    printf("PUSHS GF@$op1\n");
}

/**
 * @brief Generates a header that is same every time
 */
void generate_header() {
    printf(".IFJcode19\n");
    printf("CREATEFRAME\n");
    printf("CALL &_$INIT\n");
    printf("LABEL &_main\n");
    generate_variables_from_queue(global_var_scope, "TF");
    printf("DEFVAR GF@_result\n");
    printf("DEFVAR GF@$op1\n");
    printf("DEFVAR GF@$op2\n");
    printf("DEFVAR GF@$op1type\n");
    printf("DEFVAR GF@$op2type\n");
    printf("DEFVAR TF@_retval\n");
    printf("MOVE TF@_retval nil@nil\n");
    printf("PUSHFRAME\n");
}

/**
 * Gets called at the start of program
 */
void gen_init() {
    local_var_scope = malloc(sizeof(tDLList));
    global_var_scope = malloc(sizeof(tDLList));
    // this will most likely be different
    DLInitList(local_var_scope);
    DLInitList(global_var_scope);

    generate_header();
    // generate_variables_from_queue(global_var_scope, "GF");
    //*code_gen_stack = CDinit_stack(result);
}

char* get_line_type_string(line_type line)  {
    switch(line)    {
        case def_line: return "def_line";
        case function_call_with_assignment: return "function_call_with_assignment";
        case function_call: return "function_call";
        case assignment: return "assignment";
        case if_line: return "if_line";
        case else_line: return "else_line";
        case dedent: return "dedent";
        case return_line: return "return_line";
        case while_line: return "while_line";
        case end_of_feed: return "end_of_feed";
        default:
            printf("Detected unsupported line_type %i. Napis Markovi :^)\n", line);
    }
}

void print_gen_debug_info(line_type line) {
    DLFirst(tokenQueue);
    printf("\n~~~~~ Current line: "  );
    while(tokenQueue->Act != NULL)  {
        printf("%s ", tokenQueue->Act->value);
        DLSucc(tokenQueue);
    }
    printf("|||  line_type called: %s ~~~~~~\n", get_line_type_string(line));
}

/**
 * @brief Main function of code generator, works with global tokenQueue
 * @param line Line of Tokens representing current line in program
 */
void gen_code_from_line(line_type line) {
    Token *token = tokenQueue->Act;

    //print_gen_debug_info(line);
    prepare_line_of_tokens(tokenQueue);
    postfix_expr = TinitStack();
    switch(line)    {
        case def_line:
            scope_nesting_lvl++;
            handle_def();
            break;
        case function_call:
            DLFirst(tokenQueue);
            handle_function_call();
            break;
        case function_call_with_assignment:
            handle_function_call_with_assignment();
            break;
        case assignment:
            handle_assignment();
            break;
        case if_line:
            handle_if();
            break;
        case else_line:
            handle_else();
            break;
        case dedent:
            handle_dedent();
            break;
        case return_line:
            scope_nesting_lvl--;
            handle_return();
            break;
        case while_line:
            handle_while();
            break;
        case end_of_feed:
            handle_eof();
            break;
        default:
            break;
    }
    tokenQueue->Act = token;
}