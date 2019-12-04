/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

#ifndef IFJ_SCANNER
#define IFJ_SCANNER

#define NUMBER_OF_SPECIAL_CHARS 6
#define NUMBER_OF_OPERATORS 12
#define NUMBER_OF_KEYWORDS 7
#define NUMBER_OF_DELIMITERS 3

#define MAX_TOKEN_LEN 255
#define USING_PROG_ARGS 1


// In case you want the scanner to scan the whole file, set this value to 1
// In case you want the scanner to scan only one line of input code, set this value to 0
#define SCAN_WHOLE_FILE 0

#include "queue.h"
#include "structs.h"


typedef enum stateMachineStates	{
    STATE_START, STATE_ID, STATE_DOCSTRING, STATE_LINE_COMMENT, STATE_KEYWORD,
    STATE_OPERATOR, STATE_STRING, STATE_NUMBER, STATE_KW_ID, STATE_ERROR,
    STATE_DECIMAL_NUM, STATE_EXP_NUM
} states;

typedef enum
{
    T_COMMA,        ///Comma ,
    T_RBRACK,       ///Right bracket )
    T_EQ_ASSIG,     ///Asign =
    T_LBRACK,       ///Left bracket (
    T_COLON,        ///Colon :

    T_EQ_COMP,      ///Compare ==
    T_NEQ,          ///Not equal !=
    T_MORE,         ///More than >
    T_LESS,         ///Less than <
    T_MORE_EQ,      ///More or equal >=
    T_LESS_EQ,      ///Less or equal <=
    T_DIV,          ///Division /
    T_WH_N_DIV,     ///Whole number division //
    T_MUL,          ///Multiplication *
    T_SUB,          ///Substitution -
    T_ADD,          ///Addition +

    T_EOF,          ///End of file
    T_EOL,          ///End of line

    T_STRING,       ///String
    T_ID,           ///Identifier
    T_FLOAT,        ///Float number
    T_INT,          ///Intiger number
    T_NONE,         ///None

    T_INDENT,       ///Indent
    T_DEDENT,       ///Dedent

    T_RETURN,       ///Return
    T_WHILE,        ///While
    T_IF,           ///If
    T_ELSE,         ///Else
    T_PASS,         ///Pass
    T_DEF,          ///Def

    T_INPUTS,       ///Function inputs
    T_INPUTI,       ///Function inputi
    T_INPUTF,       ///Function inputf
    T_PRINT,        ///Function print
    T_LENGTH,       ///Function length
    T_SUBSTR,       ///Function substr
    T_ORD,          ///Function ord
    T_CHR,          ///Function chr
} T_tokenType;


// Parser-side Tokens
typedef union   {
    //dyn_id *string;
    char *string;
    int u_int;
    double u_double;
} T_tokenData;

typedef struct T_token
{
    T_tokenType type;
    T_tokenData *data;
} T_token;




int createScanner();

// The scanner's main method, used to transform the whole code into a queue of tokens
// modifies input argument
void getLineOfTokens(tDLList *tokenQueue);

// Functions used while transforming Token to T_token
T_token *token_init ();

// method that bridges the gap between getLinOfTokens and parser's getNextToken
int getParserToken(T_token *token);
void assignTokenData(T_token *out_token, Token in_token);
void assignTokenType(T_token *out_token, Token in_token);
void turnScannerTokensToParserTokens(Token *input, T_token *output);


// used to work with strings
void appendToString(char c, char string[], enum stateMachineStates state);
void returnLastCharToInput(char *buffer, char receivedChar);
void sliceString(char *string, size_t size);
char* handleEscapeSequence(char receivedChar, char *buffer);
void handleEOF(tDLList *tokenQueue, char *buffer, enum stateMachineStates *state, int positionInLine);


int isViableChar(char c);

// method that finalizes token's creation and adds it to queue
void saveTokenAndReset(tDLList *L, tokenType ttype, char *val, enum stateMachineStates *state, int positionInLine);

// methods used when checking numbers
int isDuplicateSignInNum(char *buffer);
int checkExponent (char* buffer);

int handleIndentation(char receivedChar, tDLList *queue);
int generateIndentationTokens(int i, tDLList *queue);
int generateDEDENTTokens(tDLList *queue, int i);

char get_next_char();
void return_char(char c);

int isBuiltInFunc(char* string);

//functions used to determine type of token
int isBlockComment(char* buffer);
int isBlockCommentEnd(char *buffer);
int isDelimiter(char c);
int isDigit(char c);
int isHexNum(char c);
int isKeyword(char *word);
int isLetter(char c);
int isLowercaseLetter(char c);
int isUppercaseLetter(char c);
int isOperator(char *word);
int isSpecialChar(char c);
int isWhitespace(char c);


#endif
