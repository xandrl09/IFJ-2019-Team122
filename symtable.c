#include <string.h>
#include <stdio.h>
#include "symtable.h"
#include "err.h"


/*
 * Inicializuje stack pro tabulky symbolů (scopes)
 * Na začátku v něm nic není
*/
void init_stack() {
    int stackSize = 255;
    int top = 0;
    table_stack *tmp = malloc(sizeof(symtable) + sizeof(top));
    if(tmp != NULL) {
        for (int i = 0; i < stackSize; i++) {
            tmp->sym_table[i] = NULL;
        }
        tmp->top = top;
        tmp->sym_table[0] = NULL;
        stack = tmp;
    }
    else {
        printf("Error - init_stack\n");
        errInter();
    }
}

/*
 * Inicializuje zásobník a do něho jednu tabulku symbolů a pushne ji na stack tabulek - globální scope
 *  - vlastní inicializace tabulky symbolů
*/
void init_symtable() {
    init_stack();
    symtable *table = symtable_init(255);
    TS_push(table);
}

/*
 * Inicializuje tabulku symbolů
*/
symtable* symtable_init() {
    symtable *table = malloc(sizeof(symtable));
    if (table != NULL) {
        table->size = 255;
        for(unsigned int i = 0; i < table->size; i++) {
            table->sym[i] = NULL;
        }
        return table;
    }
    else {
        printf("Error - symtable_init\n");
        errInter();
        return NULL;
    }
}

/*
 * Vytvoří symbol typu INT
 */
symbol* create_symbol_int(char* identifier, char* value) {
    symbol *sym = init_symbol();
    sym->identifier = identifier;
    sym->type = INT;
    sym->value = value;
    init_param(sym);
    sym->next_sym = NULL;
    sym->prev_sym = NULL;
    return sym;
}

/*
 * Vytvoří symbol typu FLOAT
 */
symbol* create_symbol_float(char* identifier, char* value) {
    symbol *sym = init_symbol();
    sym->identifier = identifier;
    sym->type = FLOAT;
    sym->value = value;
    init_param(sym);
    sym->next_sym = NULL;
    sym->prev_sym = NULL;
    return sym;
}

/*
 * Vytvoří symbol typu FUNCTION
 */
symbol* create_symbol_function(char* identifier) {
    symbol *sym = init_symbol();
    sym->identifier = identifier;
    sym->type = FUNCTION;
    sym->value = NULL;
    init_param(sym);
    sym->next_sym = NULL;
    sym->prev_sym = NULL;
    return sym;
}

/*
 * Vytvoří symbol typu STRING
 */
symbol* create_symbol_string(char* identifier, char* value) {
    symbol *sym = init_symbol();
    sym->identifier = identifier;
    sym->type = STRING;
    sym->value = value;
    init_param(sym);
    sym->next_sym = NULL;
    sym->prev_sym = NULL;
    return sym;
}

/*
 * Vytvoří symbol typu NIL
 */
symbol* create_symbol_nil(char* identifier) {
    symbol *sym = init_symbol();
    sym->identifier = identifier;
    sym->type = NIL;
    sym->value = NULL;
    init_param(sym);
    sym->next_sym = NULL;
    sym->prev_sym = NULL;
    return sym;
}

/*
 * Inicializace pole parametrů symbolu 'symbol'
 */
void init_param(symbol *symbol) {
    for(int i = 0; i < 255; i++) {
        symbol->param[i] = NULL;
    }
}

/*
 * Inicializace dalšího scope - CREATE_FRAME
*/
void push_lower_scope() {
    symtable *smtbl = symtable_init(255);
    TS_push(smtbl);
}

/*
 * Pushnutí tabulky na stack tabulek
 */
void TS_push(symtable *table) {
    stack->sym_table[++stack->top] = table;
}

/*
 * Popnutí tabulky ze stacku tabulek - dle potřeby možno změnit aby vracela tabulku, co popla
 */
void TS_pop() {
    // symtable *tmp = stack->sym_table[stack->top];
    symtable_delete(stack->sym_table[stack->top]);
    stack->sym_table[stack->top--] = NULL;
    // return *tmp;
}

/*
 * Zruší stack tabulek symbolů a uvolní paměť
 */
void stack_destroy() {
    for(int i = stack->top; i > 0; i--) {
        TS_pop();
    }
    free(stack);
    stack = NULL;
}

/*
 * Prohledá celý stack tabulek, jestli nenajde proměnnou 'identifier' - vrací bool
 */
bool is_in_stack(char *identifier) {
    if(search_stack(identifier) != NULL) {
        return 1;
    }
    else
        return 0;
}

/*
 * Prohledá stack, jestli se v něm nenachází symbols ID identifier
 *  - prohledá shora dolů - podle scopes
 *  - vrátí nalezení symbol
 */
symbol *search_stack(char* identifier) {

    for(int i = stack->top; i > 0; i--) {
        if(stack->sym_table[i] == NULL)
            return NULL;
        if (is_in_table(stack->sym_table[i], identifier)) {
            symbol *tmp = search_sym(stack->sym_table[i], identifier);
            return tmp;
        } else
            continue;
    }
    return NULL;
}

/*
 * Zvětšení tabulky symbolů pro rychlejší přístupy
*/
void symtable_resize(symtable *table) {
    int double_size = (table->size) * 2;
    symtable *tmp = symtable_init(double_size);
    for(int i = 0; i < table->size; i++) {
        if(table->sym[i] == NULL) {
            continue;
        }
        unsigned int hash_num = hash_function(table->sym[i]->identifier, tmp->size, SEED);
        tmp->sym[hash_num] = table->sym[i];
    }
    symtable *tmp2 = table;
    tmp->size = double_size;
    table = tmp;

    symtable_delete(tmp2);
}

/*
 * Zruší tabulku symbolů a uvolní po ní paměť
*/
void symtable_delete(symtable *target) {
    if(target != NULL) {
        for(unsigned int i = 0; i < target->size; i++) {
            if (target->sym[i] != NULL) {
                symbols_destroy(target);
            }
        }
        target->size = 0;
        free(target);
        target = NULL;
    }
}

/*
 * Vrací 1 pokud se symbol v tabulce nachází, 0 pokud ne
 */
bool is_in_table(symtable *table, char* identifier) {
    if(table != NULL || identifier != NULL) {
        if(search_sym(table, identifier) == NULL) {
            return 0;
        }
        else
            return 1;
    }
    else {
        printf("Error - is_search_sym\n");
        errInter();
        return 0;
    }
}

/*
 * Prohledá tabulku, jestli v ní je symbol s ID 'identifier' a vrátí ho, pokud existuje
 */
symbol *search_sym(symtable *table, char* identifier) {
    if(table == NULL || identifier == NULL) {
        printf("Error - search_sym\n");
        errInter();
        return 0;
    }
    else {
        unsigned int hash_num = hash_function(identifier, table->size, SEED);
        symbol* tmp = (*table).sym[hash_num];

        while (tmp != NULL) {
            if(table->sym[hash_num]->identifier != NULL) {
                if (strcmp(table->sym[hash_num]->identifier, identifier) == 0) {
                    return tmp;
                }
            }
            tmp = (*table).sym[hash_num]->next_sym;
        }
        return NULL;
    }

}

/*
 * Vloží symbol do tabulky symbolů
 *  - pokud už symbol na dané pozici je
 *      - pokud má stejný identifikátor - updatuje hodnotu
 *      - pokud ne, vloží nový symbol před existující na stejnou pozici a patřičně propojí
 */
void insert_symbol(symtable *table, symbol *sym) {
    if(is_in_table(table, sym->identifier)) {
        if (table->count >= table->size) {
            symtable_resize(table);
        }
        unsigned int hash_num = hash_function(sym->identifier, table->size, SEED);
        if (table->sym[hash_num] == NULL) {
            table->sym[hash_num] = sym;
            table->count++;
        } else {
            symbol *tmp = table->sym[hash_num];
            if (tmp->identifier == sym->identifier) {
                table->sym[hash_num]->value = tmp->value;
                return;
            }
            table->sym[hash_num]->next_sym = tmp;
            table->sym[hash_num] = sym;
            table->sym[hash_num]->next_sym->prev_sym = sym;
        }
    }
    else {
        unsigned int hash_num = hash_function(sym->identifier, table->size, SEED);
        table->sym[hash_num] = sym;
        table->count++;
    }
}

/*
 * Současné vytvoření a vložení symbolu do tabulky
 */
void create_insert_symbol( char *identifier,ST_type type, char *value) {
    if(stack != NULL) {
        if(identifier != NULL) {
            if(type == INT) {
                symbol *tmp = create_symbol_int(identifier, value);
                insert_symbol(stack->sym_table[stack->top], tmp);
            }
            else if(type == FLOAT) {
                symbol *tmp = create_symbol_float(identifier, value);
                insert_symbol(stack->sym_table[stack->top], tmp);
            }
            else if(type == FUNCTION) {
                symbol *tmp = create_symbol_function(identifier);
                insert_symbol(stack->sym_table[stack->top], tmp);
            }
            else if(type == STRING) {
                symbol *tmp = create_symbol_string(identifier, value);
                insert_symbol(stack->sym_table[stack->top], tmp);
            }
            else if(type == NIL) {
                symbol *tmp = create_symbol_nil(identifier);
                insert_symbol(stack->sym_table[stack->top], tmp);
            }
        }
    }
}

/*
 * Zničí symbol a uvolní paměť
 */
void symbol_delete(symtable *table, char* identifier) {
    unsigned int hash_num = hash_function(identifier, table->size, SEED);

    symbol *sym = search_sym(table, identifier);
    if(sym != NULL) {
        if (sym->prev_sym == NULL) {
            table->sym[hash_num] = sym->next_sym;
            if (sym->next_sym != NULL) {
                sym->next_sym->prev_sym = NULL;
            }
        } else {
            sym->prev_sym->next_sym = sym->next_sym;
            if (sym->next_sym != NULL) {
                sym->next_sym->prev_sym = sym->prev_sym;
            }
            free(sym);
        }
    }
}

/*
 * Zničí všechny symboly tabulky
 */
void symbols_destroy(symtable *table) {
    if(table != NULL) {
        for(int i = 0; i < 255; i++) {
            if(table->sym[i] != NULL) {
                if(table->sym[i]->type == FUNCTION) {
                    params_destroy(table->sym[i]);
                }
                table->sym[i]->identifier = NULL;
                table->sym[i]->type = NIL;
                table->sym[i]->value = NULL;
                table->sym[i]->next_sym = NULL;
                table->sym[i]->prev_sym = NULL;
            }
            else
                continue;
        }
    }
}

/*
 * Vytvoří rámec pro nový symbol
*/
symbol *init_symbol() {
	symbol *tmp = malloc(sizeof(symbol));
	if (tmp != NULL) {
        tmp->identifier = NULL;
        tmp->type = NIL;
        tmp->value = NULL;
        init_param(tmp);
        tmp->next_sym = NULL;
        tmp->prev_sym = NULL;
        return tmp;
    }
	else {
	    printf("Error - new_symbol\n");
	    errInter();
		return NULL;
	}
}

/*
 * Vytvoří nový parametr pro funkci - TUTAJ TO LEAKUJE
 */
parameter *new_param() {
    parameter *tmp = malloc(sizeof(parameter));
    if(tmp != NULL) {
        tmp->param_name = NULL;
        tmp->type = NIL;
        tmp->next_param = NULL;
        return tmp;
    }
    else {
        printf("Error - new_param\n");
        free(tmp);
        errInter();
        return NULL;
    }
}

/*
 * Vložení parametru s ID 'name' a typem 'type' do symbolu s ID 'dest' v aktuálníhím scope - nejvrchnější tabulce
 *  - pokud v daném scope symbol 'dest' není, prohledá vyšší scope
 */
void insert_param(char* dest, char* name, ST_type type) {
    if (dest != NULL) {
        if (name != NULL) {
            for (int i = stack->top; i > 0; i--) {
                if (is_in_table(stack->sym_table[i], dest)) {
                    symbol *dest_sym = search_sym(stack->sym_table[i], dest);
                    if(dest_sym->type == FUNCTION) {
                        if (dest_sym->param[0] == NULL) {
                            parameter *tmp = new_param();
                            dest_sym->param[0] = tmp;
                            dest_sym->param[0]->param_name = name;
                            dest_sym->param[0]->type = type;
                            dest_sym->param[0]->next_param = NULL;
                        } else {
                            int j = 0;
                            while (dest_sym->param[j] != NULL) {
                                j++;
                            }
                            parameter *tmp = new_param();
                            dest_sym->param[j] = tmp;
                            dest_sym->param[j]->param_name = name;
                            dest_sym->param[j]->type = type;
                            dest_sym->param[j]->next_param = NULL;
                        }
                    }
                    else {
                        errSemDef();
                    }
                } else {
                    continue;
                }
            }
        }
    }
}

/*
 * Vrací 1 pokud byl parameter 'identifier' nalezen pro funkci 'function'
 */
bool is_param_of(char* identifier, char *function) {
    if(identifier != NULL || function != NULL) {
        symbol *tmp = search_stack(function);
        if(tmp != NULL) {
            for(int i = 0; i < 255; i++) {
                if(tmp->param[i] == NULL) {
                    return 0;
                }
                else if (tmp->param[i]->param_name == identifier) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/*
 * Zničí parametry symbolu 'symbol
 */
void params_destroy(symbol *symbol) {
    for(int i = 0; i < 255; i++) {
        if(symbol->param[i] != NULL) {
            symbol->param[i]->param_name = NULL;
            symbol->param[i]->type = NIL;
            symbol->param[i]->next_param = NULL;
            free(symbol->param[i]);
            symbol->param[i] = NULL;
        }
        else
            break;
    }
    free(symbol->param);
}

unsigned int hash_function(char* target, unsigned int tsize, unsigned int seed) {
    char c;
    unsigned int h;
    h = seed;
    for( ; ( c=*target )!='\0' ; target++ )
    {
        h ^= ( (h << 5) + c + (h >> 2) );
    }
    return((unsigned int)((h&0x7fffffff) % tsize));
}

void print_stack() {
    printf("-----------------------------\n");
    if(stack != NULL) {
        for (int i = 0; i <= stack->top; i++) {
            if (stack->sym_table[i] != NULL) {
                printf("*** Stack item %d ***\n", i);
                for (unsigned int j = 0; j < stack->sym_table[i]->size; j++) {
                    symbol *sym = stack->sym_table[i]->sym[j];
                    if (sym != NULL) {
                        printf("ID: %s,\t Type: %d,\t Data: %s\n", sym->identifier, sym->type, sym->value);
                        if(sym->type == FUNCTION) {
                            int k = 0;
                            if(sym->param[k] == NULL) {
                                printf("No arguments for function %s\n", sym->identifier);
                                return;
                            }
                            while(sym->param[k] != NULL) {
                                if(sym->param[k]->type == INT) {
                                    printf("\tParam %d:\t Name: %s,\t Type: INT\n", k, sym->param[k]->param_name);
                                }
                                else if(sym->param[k]->type == FLOAT) {
                                    printf("\tParam %d:\t Name: %s,\t Type: FLOAT\n", k, sym->param[k]->param_name);
                                }
                                else if(sym->param[k]->type == FUNCTION) {
                                    printf("\tParam %d:\t Name: %s,\t Type: FUNCTION\n", k, sym->param[k]->param_name);
                                }
                                else if(sym->param[k]->type == STRING) {
                                    printf("\tParam %d:\t Name: %s,\t Type: STRING\n", k, sym->param[k]->param_name);
                                }
                                else if(sym->param[k]->type == NIL){
                                    printf("\tParam %d:\t Name: %s,\t Type: NIL\n", k, sym->param[k]->param_name);
                                }
                                k++;
                            }
                        }
                    } else {
                        continue;
                    }
                }
                printf("*** Konec stack items %d ***\n\n", i);
            } else
                printf("\n0. table\n");
        }
    }
    else {
        printf("*** THIS IS NOT THE STACK YOU ARE LOOKING FOR ***\n\n");
    }
    printf("-----------------------------\n");
}