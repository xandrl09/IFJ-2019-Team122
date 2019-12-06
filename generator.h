/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#ifndef PROJEKT_GENERATOR_H
#define PROJEKT_GENERATOR_H

#include "structs.h"
#include <stdlib.h>
#include "scanner.h"
#include "stack_cd.h"
#include "stack.h"

#define MAX_LINE_LENGTH 1024

/** STANDARDS
 * Starting labels are marked with &
 * Closing labels are marked with *
 * Function arguments are marked with % and appropriate number, eg. %1, %2 etc.0
 * Return value is always called _retval
 * Flow control labels are marked with [&,*]_$, eg. &_$IF0, *_$WHILE32
 * &_$bool1 - used in gen_expression_to_bool, only one instance
 * &_$nil1 - used in gen_expression_to_bool, only one instance
 * $op1, $op2, $op1type, $op2type
 *
 * LABEL $int_float42
 * LABEL $float_int42
 * LABEL $type_check_passed na konci typove kontroly operandu
 * LABEL &else42 a *else42
 * LABEL &_$INIT
 * LABEL &foo_INIT
 * LABEL *foo_INIT
 * LABEL &$inputi a *$inputi, obdobne pro inputs a inputf
 */



typedef enum    {
    def_line, function_call, function_call_with_assignment, assignment,
    if_line, else_line, dedent, return_line, while_line, end_of_feed
}line_type;


int is_relational_op(char* val);
void gen_print_method(int num_of_args);
void gen_init();
void gen_code_from_line(line_type line);
void generate_header();
void generate_variables_from_queue(tDLList *queue, char* scope);
void prepare_line_of_tokens(tDLList *queue);
void gen_var_declaration_code(Token *token, char* scope);
void generate_variable(char* name, char* scope, tDLList *queue);
void gen_function_def_args();
void generate_expression();
int gen_function_call_args();
void gen_switch_operands();
void gen_assign_types();
void gen_typecheck_jumps();

char* get_type_from_value(Token *token);

char* get_variable_scope(char* arg);

int is_duplicate_definition(char *identifier, tDLList *queue);

void gen_op1_is_int();
void gen_op1_is_float();
void gen_op1_is_string();
void gen_ops_are_int(char* operator_value);
void gen_ops_are_float(char *operator_value);
void gen_ops_are_string(char *operator_value);
void gen_int_float(char* operator_value);
void gen_float_int(char* operator_value);

void gen_actual_computation(char* operator);
void gen_expression_operation(char *operator_value);

tDLList get_expression_queue(int to_skip);
void handle_return();
void handle_def();
void handle_dedent();
void handle_function_call();
void handle_function_call_with_assignment();
void handle_assignment();
void handle_if();
void handle_while();
void handle_else();
void handle_inputi_line();
void handle_inputf_line();
void handle_inputs_line();
void handle_eof();

void gen_relational_comparison(char* op);

#endif //PROJEKT_GENERATOR_H