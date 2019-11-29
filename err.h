/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#ifndef ERR
#define ERR

#define errLEX 1
#define errSYN 2
#define errSEMDEF 3
#define errSEMTYPE 4
#define errSEMWRONGPARAM 5
#define errSEMOTH 6
#define errZERO 9
#define errINTER 99

// THE FOLLOWING CODE HAS BEEN STOLEN FROM Roki's project
#define SCANNER_TOKEN_OK			            0 /// Token is OK.
#define SYNTAX_OK 					            0 /// Syntax is OK.
#define SCANNER_ERROR_LEX			            1 ///  chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
#define SYNTAX_ERROR					        2 /// chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu).
#define SEM_ERR_UNDEFINED_VAR		            3 /// sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefi-nici funkce/proměnné, atp.
#define SEM_ERR_TYPE_INCOMPATIBILITY			4 /// émantická/běhová chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
#define SEM_ERR_WRONG_FUNC_PARAMS               5 /// sémantická chyba v programu – špatný počet parametrů u volání funkce.
#define SEM_ERR_OTHER				            6 /// Semantic error - other.
#define DIVIDE_ZERO                             9 /// Run error - divide with zero
#define ERROR_INTERNAL				            99 /// Internal error, eg. malloc error etc.


void errLex();
void errSyn();
void errSemDef();
void errSemType();
void errSemWrongParam();
void errSemOth();
void errZero();
void errInter();

#endif
