/**
 * @author Tomáš Studýnka
 * @package IFJ Project 2018
 * @version Revision 1.0
 * @brief Dynamic string data type and it's functions for creating identifiers of unspecified length
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynid.h"

typedef struct list_string{
    char item;
    struct list_string *next;
}list_string;


typedef struct dyn_id{
    unsigned long len;
    list_string *dynid;

}dyn_id;


dyn_id *dyn_id_init();

int dyn_id_append_char(dyn_id *target, char c);

int dyn_id_strcat(dyn_id *target, const char *constr);


int dyn_id_cmp(dyn_id *a, dyn_id *b);

void print_dyn_id(dyn_id *target);


int parse_dyn_id_to_int(dyn_id *target);

int dyn_id_cmp_const(dyn_id *a, const char *constr);


void dyn_id_destroy(dyn_id *target);

dyn_id *dyn_id_from_constr(const char *c);

double parse_dyn_id_to_double(dyn_id *target);

unsigned long dyn_id_strlen(dyn_id *target);
