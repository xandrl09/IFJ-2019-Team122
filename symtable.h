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

void init_symtable();
void init_next_scope();
void init_stack();
void stack_destroy();
void TS_push(symtable *table);
void TS_pop();
symtable* symtable_init(int size);
void symtable_delete(symtable *target);
symbol* create_symbol_int(char* identifier, char* value);
symbol* create_symbol_float(char* identifier, char* value);
symbol* create_symbol_function(char* identifier);
symbol* create_symbol_string(char* identifier, char* value);
symbol* create_symbol_nil(char* identifier);
void insert_symbol(symtable *table, symbol *sym);
void create_insert_symbol(char *identifier,ST_type type, char *value);
void insert_param(char* dest, char* name, ST_type type);
bool is_in_table(symtable *table, char* identifier);
symbol *search_sym(symtable *table, char* identifier);
symbol *search_stack(char* identifier);
symbol *new_symbol();
parameter *new_param();
void init_param(symbol *symbol);
void symbols_destroy(symtable *table);
void symbol_delete(symtable *table, char* identifier);
unsigned int hash_function(char* target, unsigned int size, unsigned int seed);
void print_stack();
