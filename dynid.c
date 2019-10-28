/**
 * @authors Tomáš Studýnka, Kryštof Slanina
 * @package IFJ Project 2018
 * @version Alpha, WIP
 * @brief Symbol table library for the compiler project
 * */
/**
* Library of functions for operations on dynamically allocated identifiers of indefinite length
*@bug functions dyn_id_cmp and dyn_id_cmp_const are currently not functionally equivalent to strcmp,
* although they can recognize equality just fine
* @note Fixed segfaults, current implementation appears to be functional
*
* */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "dynid.h"
#include "err.h"


/**Dynamic identifier constructor
 */

dyn_id *dyn_id_init(){
    dyn_id *new = malloc(sizeof(dyn_id));

    if(new){
        new->dynid = NULL;
        new->len = 0;
        return new;
    }
    return NULL;

}

int parse_dyn_id_to_int(dyn_id *target) {
    char  *buffer = malloc(sizeof(char) * dyn_id_strlen(target) + 1);
    if(buffer){
        list_string *tmp = target->dynid;
        int idx = 0;
        while (tmp != NULL){

            sprintf(&buffer[idx], "%c", tmp->item);
            ++idx;
            tmp = tmp->next;

        }
        char *err;
        int toret = (int)strtol(buffer, &err, 10);

        if (*err != '\0' || errno == ERANGE){
            free(buffer);
            exit(ERROR_INTERNAL);
        }
        else{
            free(buffer);
            return toret;
        }


    }
    else{
        exit(99);
    }
}

double parse_dyn_id_to_double(dyn_id *target){
    char * buffer = malloc(sizeof(char) * dyn_id_strlen(target) + 1);
    if(buffer){
        list_string *tmp = target->dynid;
        int idx = 0;
        while (tmp != NULL){

            sprintf(&buffer[idx], "%c", tmp->item);
            ++idx;
            tmp = tmp->next;
        }
        char *err;
        double toret = strtod(buffer, &err);
        if (*err != '\0' || toret == HUGE_VAL){
            free(buffer);
            exit(ERROR_INTERNAL);
        }
        else{
            free(buffer);
            return toret;
        }
    }
}

static list_string *list_string_init(){
    list_string *new = malloc(sizeof(list_string));

    if(new){
        new->next = NULL;
        return new;
    }
    return NULL;
}
static int dyn_id_write_char(list_string *target, char c){
    if (target){
        target->item = c;
        return 0;
    }
    return 1;

}
/**Appends one character to the end of the dynamic identifier
 */

int dyn_id_append_char(dyn_id *target, char c) {
    list_string *string  = target->dynid;
    if (string) {
        while (string->next != NULL) {
            string = string->next;
        }
        if(string->item == '\0') {
            dyn_id_write_char(string, c);
            string->next = list_string_init();
            dyn_id_write_char(string->next, '\0');
            target->len = target->len + 1;
            return 0;
        }
        else {
            string->next = list_string_init();
            dyn_id_write_char(string->next, c);
            string->next->next = list_string_init();
            dyn_id_write_char(string->next->next, '\0');
            target->len = target->len + 1;
            return 0;
        }
    }
    else{
        target->dynid = list_string_init();
        if (target->dynid){
            dyn_id_write_char(target->dynid, c);
            target->len = target->len + 1;
            target->dynid->next = list_string_init();
            if(target->dynid->next){
                dyn_id_write_char(target->dynid->next, '\0');
            }
            return 0;

        } else return 1;

    }

    return 1;
}

/**Concatenates a dynamic identifier with a constant string
 */
int dyn_id_strcat(dyn_id *target, const char *constr){

    const char *string = constr;
    while (*string != '\0'){
        dyn_id_append_char(target, *string);
        ++string;


    }
    return 0;
}

/**Compares two dynamic identifiers lexicographically.
 * Implementation should be functionally equivalent to strcmp in glibc
 */
int dyn_id_cmp(dyn_id *a, dyn_id *b){
    list_string *tmp1 = a->dynid;
    list_string *tmp2 = b->dynid;
    unsigned char c1 = 0, c2 = 0;

    while (tmp1 != NULL && tmp2 != NULL){
        c1 = (unsigned char) tmp1->item;
        c2 = (unsigned char) tmp2->item;

        if((c1 == '\0' || c2 == '\0') || c1 != c2){
            return c1 - c2;
        }

        tmp1 =tmp1->next;
        tmp2= tmp2->next;


    }
    return c1 - c2;
}

/**Compares a dynamic identifier with a constant string lexicographically.
 * The implementation should again be functionally equivalent to stcmp in glibc
 */
int dyn_id_cmp_const(dyn_id *a, const char *constr){
    list_string *tmp = a->dynid;
    const unsigned char *s = (const unsigned char *) constr;
    unsigned char c1 = 0, c2 = 0;
    while(tmp != NULL){
        c1 = (unsigned char) tmp->item;
        c2 = *s;
        if ((c1 == '\0' || c2 == '\0') || c1 != c2){
            return c1 - c2;
        }

        tmp =tmp->next;
        ++s;
    }


    return c1 - c2;
}


/**Destructor for dynamic identifier, frees all memory
 */
void dyn_id_destroy(dyn_id *target) {
    if (target){
        if (target->dynid == NULL) {
            free(target);
            return;
        }
        list_string *next = target->dynid->next;
        list_string *curr = target->dynid;
        while (next != NULL) {
            free(curr);
            curr = next;
            next = next->next;

        }
        free(curr);
        free(target);
    }
}
dyn_id *dyn_id_from_constr(const char *c){
    dyn_id *tmp = dyn_id_init();
    while (*c!= '\0') {
        dyn_id_append_char(tmp, *c);

        ++c;

    }
    return tmp;

}

unsigned long dyn_id_strlen(dyn_id *target){
    if (target){
        return target->len;
    }
    return 0;
}
void print_dyn_id(dyn_id *target) {

    list_string *tmp = target->dynid;
    if (tmp) {
        while (tmp->next != NULL) {
            printf("%c", tmp->item);
            tmp = tmp->next;
        }
        printf("%c", tmp->item);

    } else {
        printf("%c", '\0');
    }
}