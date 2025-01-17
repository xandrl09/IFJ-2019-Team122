/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#ifndef IFJ_STRUCTS
#define IFJ_STRUCTS




typedef enum tokenTypes {
    error, keyword, identifier, integer, floatingPoint, builtInFunc,
    Operator, string, specialChar, INDENT, DEDENT, EOL, EoF, docString, nil
} tokenType;

typedef struct Token {
    int positionInLine;
    tokenType type;
    struct Token *lptr;
    struct Token *rptr;
    char value[255];
} Token;

typedef struct tDLList  {                                  /* dvousměrně vázaný seznam */
    Token *First;                      /* ukazatel na první prvek seznamu */
    Token *Act;                     /* ukazatel na aktuální prvek seznamu */
    Token *Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

typedef struct Stack    {
    int top;
    int arr[255];
} Stack;

// Stack used to convert infix expressions to postfix
typedef struct TStack   {
    int top;
    Token* arr[255];
} TStack;

// Stack used to track lines of code to be printed
typedef struct CDStack  {
    int top;
    char* arr[255];
} CDStack;

#endif