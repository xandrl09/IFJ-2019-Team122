#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

/** STANDARDS
 * Starting labels are marked with &
 * Closing labels are marked with *
 * Function arguments are marked with % and appropriate number, eg. %1, %2 etc.0
 * Return value is always called _retval
 * Flow control labels are marked with [&,*]_$, eg. &_$IF0, *_$WHILE32
 * &_$bool1 - used in gen_expression_to_bool
 * &_%nil1 - used in gen_expression_to_bool
 */

// This queue of tokens is taken directly from scanner
extern tDLList *tokenQueue;

// Stack used to track lines of IFJcode19 that generator cannot print right away
CDStack code_gen_stack;

// This queue of tokens is taken from global symtable
tDLList *global_var_scope;
// This queue of toknes is taken from local symtable (changes quite often)
tDLList *local_var_scope;

int while_lbl_id = 0;

int bool_lbl_id = 0;
int nil_lbl_id = 0;

// ..... this doesn't work at all. This doesn't make actual sense
/**
 * @brief After calling this function, program generates a block of code that handles assigning true/false to _result
 */
void gen_bool_comp_func()   {
    printf("LABEL &_BOOL_CMP\n");                           // start of the function
    printf("TYPE GF@op1type GF@_result\n");                 // save _result's type
    printf("JUMPIFEQ &_IS_BOOL GF@op1type string@bool\n");  // if token already is bool, skip to the end of func
    printf("JUMPIFEQ &_IS_NIL GF@op1type string@nil\n");    // if token is nil, jump to nil-block
    printf("MOVE GF@_result bool@true\n");                  // if token is none of the above, assign bool@true to _result
    printf("JUMP &_IS_BOOL\n");                             // variable is now bool, can skip to the end of function
    printf("LABEL &_IS_NIL\n");                             // in case variable is nil
    printf("MOVE GF@_result bool@false\n");                 // set the result's value to false
    printf("JUMP &_IS_BOOL\n");                             // skip to the end of function
    printf("LABEL &_IS_BOOL\n");                            // end of function
    printf("RETURN\n");
}

// TODO THIS RIGHT HERE
void gen_expression_to_bool()   {
    printf("TYPE GF@op1type GF@_result\n");

}

char* get_bool_lbl() {
    char *ret = "";
    sprintf(ret, "&_$bool%i", nil_lbl_id++);
    return ret;
}

/**
 * @brief Prints opening label and pushes closing label to code_gen_stack
 */
void gen_while_labels() {
    char* label = get_while_lbl();
    printf("LABEL &%s\n", label);
    char* tmp = sprintf(tmp, "LABEL *%s\n", label);
    CDpush(&code_gen_stack, tmp);
}

// TODO current
void handle_while_line()    {
    gen_while_labels();
    generate_expression(tokenQueue->First->rptr);
    gen_expression_to_bool(); // this turns our expression's result to either true or false




    CDpop(&code_gen_stack);
}


/**
 * variable = 4
 * variable = 4 + 3
 * variable = a + f
 */
void handle_assignemnt_line()    {
    generate_expression(tokenQueue->First->rptr->rptr);
    if (is_var_local(tokenQueue->First->value))
        printf("MOVE LF@%s GF@_result\n", tokenQueue->First->value);


}

/**
 * When program meets the line return my_var, this gets called
 */
void handle_return_line()   {
    if (local_var_scope->First->rptr != NULL &&
        local_var_scope->First->rptr->type != EOL &&
        local_var_scope->First->rptr->type != EoF)  {

        if(is_var_local(local_var_scope->First->rptr->value))
            printf("MOVE LF@_retval LF@%s\n", local_var_scope->First->rptr->value);
        else
            printf("MOVE LF@_retval GF@%s\n", local_var_scope->First->rptr->value);
    }
    printf("%s", CDpop(&code_gen_stack));
}

void handle_def_line() {
    printf("JUMP *%s\n", tokenQueue->First->rptr->value); // jump to the end of function
    printf("LABEL &%s\n", tokenQueue->First->rptr->value);
    printf("PUSHFRAME\n");
    generate_variable("_retval", "LF", local_var_scope);
    gen_function_def_args();

    // The function's code itself

    // pushes lines of code to be printed later
    CDpush(&code_gen_stack, "POPFRAME\nRETURN\n");
}

void handle_dedent()   {
    printf("%s", CDpop(&code_gen_stack));
}

void handle_function_call() {
    printf("CREATEFRAME\n");
    gen_function_call_args();
    printf("CALL &%s\n", tokenQueue->First->value);
    printf("MOVE LF@_retval TF@_retval\n");
}


/**
 * @brief used to generate a while label, eg. _$WHILE0, _$WHILE23
 */
char* get_while_lbl()    {
    char* ret = sprintf(ret, "_$WHILE%i", while_lbl_id++);
    return ret;
}


/**
 * Determines whether given variable's name is present in local scope
 * @param name Variable name
 * @return Returns either 1 or 0
 */
int is_var_local(char *name) {
    DLFirst(local_var_scope);
    while(local_var_scope->Act != NULL) {
        if (strcmp(local_var_scope->Act->value, name) == 0)
            return 1;
    }
    return 0;
}

// TODO
char* get_type_from_value(Token *token)    {
    switch(token->type) {
        case floatingPoint: return "float"; break;
        case integer: return "int"; break;
        case string: return "string"; break;
        default: fprintf(stderr, "Encountered some strange token in generator, type: %i\n", token->type);
    }
}

/**
 * Generates both tokens and code for function's arguments at the start of a function
 * Function signature looks like this: def foo(arg1, arg2):
 */
void gen_function_def_args() {
    DLFirst(tokenQueue);
    tokenQueue->Act = tokenQueue->First->rptr->rptr->rptr; // first argument is now active
    int arg_cnt = 0;
    while(tokenQueue->Act != NULL && tokenQueue->Act->type == specialChar)  {
        printf("DEFVAR LF@%s\n", tokenQueue->Act->value);
        printf("MOVE LF@%s LF@%%%i\n", tokenQueue->Act->value, arg_cnt++);
        generate_variable(tokenQueue->Act->value, "LF", local_var_scope);
        DLSucc(tokenQueue);
        DLSucc(tokenQueue);
    }
}

/**
 *  my_function(argument1, argument22):
 */
void gen_function_call_args()   {
    DLFirst(tokenQueue);
    tokenQueue->Act = tokenQueue->First->rptr->rptr;    // we're pointing at first argument
    int arg_cnt = 0;
    while (tokenQueue->Act != NULL && tokenQueue->Act->type != specialChar && tokenQueue->Act->type  != EOL) {
        printf("DEFVAR TF@%%%i\n", ++arg_cnt);
        printf("MOVE TF@%%%i %s@%s\n", arg_cnt, get_type_from_value(tokenQueue->Act), tokenQueue->Act->value);
        DLSucc(tokenQueue);
        DLSucc(tokenQueue);
    }
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
    }
}

/**
 * @brief This function is called for global variables and then for each function definition
 * @param queue Queue of tokens from symtable for current scope
 * @param scope Current scope (GF, TF, LF)
 */
void generate_variables_from_queue(tDLList *queue, char *scope) {
    DLFirst(queue);
    while(queue->Act != NULL)    {
        gen_var_declaration_code(queue->Act, scope);
        DLSucc(queue);
    }
}

char* gen_bool_lbl()    {
    //char* ret = sprintf(ret, "")
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




// TODO - bigass fucking todo
/**
 * @brief This function generates code from CLEARS to POPS GF@_result
 * @param active_token First token in the expression, it's a token from tokenQueue
 */
void generate_expression(Token *active_token) {

}

/**
 * @brief Generates a header that is same every time
 */
void generate_header() {
    printf(".IFJcode19\n");
    printf("CREATEFRAME\n");
    printf("LABEL &_main\n");
    printf("DEFVAR GF@_result\n");
    DLInsertLast(global_var_scope, nil, "_result", -1);
    printf("DEFVAR GF@op1\n");
    DLInsertLast(global_var_scope, nil, "op1", -1);
    printf("DEFVAR GF@op2\n");
    DLInsertLast(global_var_scope, nil, "op2", -1);
    printf("DEFVAR GF@op1type\n");
    DLInsertLast(global_var_scope, nil, "op1type", -1);
    printf("DEFVAR GF@op2type\n");
    DLInsertLast(global_var_scope, nil, "op2type", -1);
    generate_variable("_retval", "TF", global_var_scope);
    printf("PUSHFRAME\n");
}

/**
 * Gets called at the start of program
 */
void gen_init() {
    local_var_scope = malloc(sizeof(tDLList));
    global_var_scope = malloc(sizeof(tDLList));
    DLInitList(local_var_scope);
    DLInitList(global_var_scope);
    generate_header();
    printQueueContents(global_var_scope);
   // generate_variables_from_queue(global_var_scope, "GF");
    code_gen_stack = CDinit_stack();
}

/**
 * @brief Main function of code generator, works with global tokenQueue
 * @param line Line of Tokens representing current line in program
 */
void gen_code_from_line(line_type line) {
    prepare_line_of_tokens(tokenQueue);
    switch(line)    {
        case def_line:
            handle_def_line();
            break;
        case function_call:
            handle_function_call();
            break;
        case function_call_with_assignment: // TODO
            break;
        case assignment:  // TODO
            //handle_assignment();
            break;
        case if_line:    // TODO
            break;
        case else_line:  // TODO
            break;
        case dedent:
            handle_dedent();
            break;
        case return_line:
            handle_return_line();
            break;
        case while_line:     // TODO
            handle_while_line();
            break;
        default:
            break;
    }
}

