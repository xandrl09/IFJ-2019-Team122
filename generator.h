//
// Created by Marek on 11/23/2019.
//

#ifndef PROJEKT_GENERATOR_H
#define PROJEKT_GENERATOR_H

#include "structs.h"
#include <stdlib.h>
#include "scanner.h"
#include "stack_cd.h"

#define MAX_LINE_LENGTH 1024


typedef enum    {
    def_line, function_call, function_call_with_assignment, assignment,
    if_line, else_line, dedent, return_line, while_line
}line_type;


void gen_init();
void gen_code_from_line(line_type line);
void generate_header();
void generate_variables_from_queue(tDLList *queue, char* scope);
void prepare_line_of_tokens(tDLList *queue);
void gen_var_declaration_code(Token *token, char* scope);
void generate_variable(char* name, char* scope, tDLList *queue);
void gen_function_def_args();
void generate_expression(Token *active_token); // TODO
void gen_function_call_args();
int is_var_local(char *name);
char* get_while_lbl();
//char* gen_bool_lbl();
//char* gen_nil_lbl();

void handle_return_line();
void handle_def_line();
void handle_dedent();
void handle_function_call();

void handle_assignment();
void handle_while_line(); // TODO
#endif //PROJEKT_GENERATOR_H
