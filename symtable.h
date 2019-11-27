#include <stdbool.h>
#include <stdlib.h>

#define SEED 1159241

#pragma once

struct symbol;
struct symtable;
struct table_stack *stack;


typedef struct table_stack {
    int top;
    struct symtable *sym_table[255];
}table_stack;

typedef enum ST_type {
    INT,
    FLOAT,
    FUNCTION,
    STRING,
    NIL
} ST_type;

typedef struct parameter {
	char* param_name;
	ST_type type;
	struct parameter *next_param;
} parameter;

typedef struct symbol {
	char* identifier;
    ST_type type;
    char* value;
	parameter *param[255];
	struct symbol *next_sym;
	struct symbol *prev_sym;
} symbol;

typedef struct symtable {
    int size;
	symbol *sym[255];
	unsigned int count;
} symtable;

/*
 * Inicializační funkce
 */
void init_stack();
void init_symtable();
symtable* symtable_init(int size);
symbol *init_symbol();
symbol* create_symbol_int(char* identifier, char* value);
symbol* create_symbol_float(char* identifier, char* value);
symbol* create_symbol_function(char* identifier);
symbol* create_symbol_string(char* identifier, char* value);
symbol* create_symbol_nil(char* identifier);
void init_param(symbol *symbol);

/*
 * Práce se stackem tabulek
 */
void push_lower_scope();
void TS_push(symtable *table);
void TS_pop();
void stack_destroy();
bool is_in_stack(char *identifier);
symbol *search_stack(char* identifier);

/*
 * Práce s tabulkou symbolů
 */
void symtable_resize(symtable *table);
void symtable_delete(symtable *target);
bool is_in_table(symtable *table, char* identifier);
symbol *search_sym(symtable *table, char* identifier);

/*
 * Práce se symboly
 */
void in_sym(symtable *table, symbol *sym);
void insert_symbol(char *identifier, ST_type type, char *value); // = in_sym + create_symbol
void symbol_delete(symtable *table, char* identifier);
void symbols_destroy(symtable *table);

/*
 * Práce s parametry
 */
parameter *new_param();
void insert_param(char* dest, char* name, ST_type type);
bool is_param_of(char* identifier, char *function);
void params_destroy(symbol *symbol);

/*
 * Hashovací funkce
 */
unsigned int hash_function(char* target, unsigned int size, unsigned int seed);

/*
 * Vytiskne stack a jeho obsah
 */
void print_stack();
