/**
 * Implementace překladače imperativního jazyka IFJ19
 *
 * xlinne00     Linner Marek
 * xstoja06     Stojan Martin
 * xandrl09     Andrla Ondřej
 *
 */

// TODO - escape sekvence nejsou updatovane pro soucasny struct Token


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "scanner.h"
#include "err.h"
#include "stack.h"

Stack indentationStack;
extern tDLList *tokenQueue; // this is necessary because of scanner-parser interface

static char *keywords[NUMBER_OF_KEYWORDS] = {"def", "else", "if", "None", "pass", "return", "while"};
static char delimiters[NUMBER_OF_DELIMITERS] = {' ', '\t', '\n'};
static char *operators[NUMBER_OF_OPERATORS] = {"+", "-", "*", "/", "//", "=", "==", "<=", ">=", "!=", ">", "<"};
static char specialChars[NUMBER_OF_SPECIAL_CHARS] = {'(', ')', '[', ']', ',', ':'};
static FILE *inputFile;


int createScanner(char *path) {
    inputFile = fopen(path, "r");
    if (inputFile == NULL) {
        fprintf(stderr, "File cannot be opened!\n");
        return -1;
    }
    indentationStack = initStack();
    return 0;
}

// This function is in fact and interface between scanner and parser. It modifies its input parameter
// Basically this returns a Token from our global tokenQueue, if the token's type is EOL, it scans another line
int getParserToken(T_token *token) {
    if (tokenQueue == NULL || tokenQueue->First == NULL)
        getLineOfTokens(tokenQueue);

    if(tokenQueue->Act == NULL) {
        DLFirst(tokenQueue);
    }
    turnScannerTokensToParserTokens(tokenQueue->Act, token);
    //printf("Token %s parsed.\n", tokenQueue->Act->value);
    DLSucc(tokenQueue);

    if (tokenQueue->Act == NULL) {
        printQueueContents(tokenQueue);
        getLineOfTokens(tokenQueue);
    }
}


void turnScannerTokensToParserTokens(Token *input, T_token *output) {
    assignTokenData(output, *input);
    assignTokenType(output, *input);
}

void assignTokenData(T_token *out_token, Token in_token) {
    switch (in_token.type) {
        case integer:
            out_token->data->u_int = atoi(in_token.value);
            break;
        case floatingPoint:
            out_token->data->u_double = atol(in_token.value);
            break;
        default:
            out_token->data->string = in_token.value;
    }
}


void assignTokenType(T_token *out_token, Token in_token) {
    switch (in_token.type) {
        case error:
            errLex();
            break;
        case keyword:
            if (strcmp(in_token.value, "return") == 0)
                out_token->type = T_RETURN;
            else if (strcmp(in_token.value, "while") == 0)
                out_token->type = T_WHILE;
            else if (strcmp(in_token.value, "if") == 0)
                out_token->type = T_IF;
            else if (strcmp(in_token.value, "else") == 0)
                out_token->type = T_ELSE;
            else if (strcmp(in_token.value, "pass") == 0)
                out_token->type = T_PASS;
            else if (strcmp(in_token.value, "def") == 0)
                out_token->type = T_DEF;
            else if (strcmp(in_token.value, "None") == 0)
                out_token->type = T_NONE;
            break;
        case identifier:
            out_token->type = T_ID;
            break;
        case integer:
            out_token->type = T_INT;
            break;
        case floatingPoint:
            out_token->type = T_FLOAT;
            break;
        case builtInFunc:
            if (strcmp(in_token.value, "inputs") == 0)
                out_token->type = T_INPUTS;
            else if (strcmp(in_token.value, "intputi") == 0)
                out_token->type = T_INPUTI;
            else if (strcmp(in_token.value, "intputf") == 0)
                out_token->type = T_INPUTF;
            else if (strcmp(in_token.value, "print") == 0)
                out_token->type = T_PRINT;
            else if (strcmp(in_token.value, "length") == 0)
                out_token->type = T_LENGTH;
            else if (strcmp(in_token.value, "substr") == 0)
                out_token->type = T_SUBSTR;
            else if (strcmp(in_token.value, "ord") == 0)
                out_token->type = T_ORD;
            else if (strcmp(in_token.value, "chr") == 0)
                out_token->type = T_CHR;
            break;
        case Operator:
            if (strcmp(in_token.value, "=") == 0)
                out_token->type = T_EQ_ASSIG;
            else if (strcmp(in_token.value, "==") == 0)
                out_token->type = T_EQ_COMP;
            else if (strcmp(in_token.value, "!=") == 0)
                out_token->type = T_NEQ;
            else if (strcmp(in_token.value, ">") == 0)
                out_token->type = T_MORE;
            else if (strcmp(in_token.value, "<") == 0)
                out_token->type = T_LESS;
            else if (strcmp(in_token.value, ">=") == 0)
                out_token->type = T_MORE_EQ;
            else if (strcmp(in_token.value, "<=") == 0)
                out_token->type = T_LESS_EQ;
            else if (strcmp(in_token.value, "/") == 0)
                out_token->type = T_DIV;
            else if (strcmp(in_token.value, "//") == 0)
                out_token->type = T_WH_N_DIV;
            else if (strcmp(in_token.value, "*") == 0)
                out_token->type = T_MUL;
            else if (strcmp(in_token.value, "-") == 0)
                out_token->type = T_SUB;
            else if (strcmp(in_token.value, "+") == 0)
                out_token->type = T_ADD;
            break;
        case string:
            out_token->type = T_STRING;
            break;
        case specialChar:
            if (strcmp(in_token.value, ",") == 0)
                out_token->type = T_COMMA;
            else if (strcmp(in_token.value, ")") == 0)
                out_token->type = T_RBRACK;
            else if (strcmp(in_token.value, "(") == 0)
                out_token->type = T_LBRACK;
            else if(strcmp(in_token.value, ":") == 0)
                out_token->type = T_COLON;
            break;
        case INDENT:
            out_token->type = T_INDENT;
            break;
        case DEDENT:
            out_token->type = T_DEDENT;
            break;
        case docString:
            out_token->type = T_STRING;
            break;
        case EoF:
            out_token->type = T_EOF;
            break;
        case EOL:
            out_token->type = T_EOL;
            break;
        default:
            errLex();
    }
}

T_token *token_init() {
    struct T_token *token = malloc(sizeof(T_token));
    token->data = malloc(sizeof(T_tokenData));
    return token;
}

void appendToString(char c, char string[], enum stateMachineStates state) {
    if (state != STATE_STRING && state != STATE_DOCSTRING) {
        if (isspace(c) || c == '\n') { //potencialni chyba
            return;
        }
    }
    for(int i = 0; i < MAX_TOKEN_LEN - 1; i++) {
        if (string[i] == '\0') {
            string[i] = c;
            string[i+1] = '\0';
            break;
        }
    }
}

void returnLastCharToInput(char *buffer, char receivedChar) {
    if (receivedChar == '\r')
        return;
    ungetc(receivedChar, inputFile);
    sliceString(buffer, strlen(buffer) - 1);
}

// how many tokens should be preserved, the rest is cut away
void sliceString(char *string, size_t size) {
    for(int i = 0; i < MAX_TOKEN_LEN; i++)  {
        if (string[i] == '\0' && i > 0) {
            string[i-1] = '\0';
            break;
        }
    }
}


int isBuiltInFunc(char *string) {
    char *builtInFunc[8] = {"inputi", "inputf", "inputs", "print", "length", "substr", "ord", "chr"};
    for (int i = 0; i < 8; i++) {
        if (strcmp(string, builtInFunc[i]) == 0)
            return 1;
    }
    return 0;
}

void stripDocString(char* val) {
    for(int i = 0; i < strlen(val); i++)
        val[i] = val[i+3];
    int cnt = 0;
    for(int i = strlen(val) - 1;; i--)  {
        val[i] = '\0';
        cnt++;
        if (cnt == 3)
            break;
    }
}

void saveTokenAndReset(tDLList *L, tokenType ttype, char *val, enum stateMachineStates *state, int positionInLine) {
    if (isBuiltInFunc(val))
        ttype = builtInFunc;

    if (ttype == docString)
        stripDocString(val);

    DLInsertLast(L, ttype, val, positionInLine);
    if (ttype != INDENT && ttype != DEDENT)
        *state = STATE_START;
    if (strcmp(val, "") != 0)
        strcpy(val, "");
    L->Act = L->First;
}

int checkExponent(char *buffer) {
    int count = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == 'e' || buffer[i] == 'E') {   //zacina exponent
            for (int x = 1; x < 3; x++) {
                if (!isDigit(buffer[i + 1]) && (buffer[i + 1] != '+' && buffer[i + 1] != '-'))
                    return false;
                else {
                    if (buffer[i + 1] == '0') {
                        if (isDigit(buffer[i + 2]))
                            return false;
                        else
                            return true;
                    }
                }
            }
        }
    }
    return true;
}

int isDuplicateSignInNum(char *buffer) {
    bool hasSign = false;
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '+' || buffer[i] == '-') {
            if (hasSign)
                return true;
            else
                hasSign = true;
        }
    }
    return false;
}

void handleEOF(tDLList *tokenQueue, char *buffer, enum stateMachineStates *state, int positionInLine) {
    switch (*state) {
        case STATE_KEYWORD:
            saveTokenAndReset(tokenQueue, keyword, buffer, state, positionInLine++);
            break;
        case STATE_ID:
            saveTokenAndReset(tokenQueue, identifier, buffer, state, positionInLine++);
            break;
        case STATE_NUMBER:
            saveTokenAndReset(tokenQueue, integer, buffer, state, positionInLine++);
            break;
        case STATE_DECIMAL_NUM:
            saveTokenAndReset(tokenQueue, floatingPoint, buffer, state, positionInLine++);
            break;
        case STATE_EXP_NUM:
            saveTokenAndReset(tokenQueue, floatingPoint, buffer, state, positionInLine++);
            break;
        case STATE_OPERATOR:
            if (isOperator(buffer)) {
                saveTokenAndReset(tokenQueue, Operator, buffer, state, positionInLine++);
            } else {
                saveTokenAndReset(tokenQueue, error, buffer, state, positionInLine++);
            }
            break;
        case STATE_KW_ID:
            if (isKeyword(buffer)) {
                saveTokenAndReset(tokenQueue, keyword, buffer, state, positionInLine++);
            } else {
                saveTokenAndReset(tokenQueue, identifier, buffer, state, positionInLine++);
            }
            break;
    }
    generateDEDENTTokens(tokenQueue, 0);
    if (tokenQueue->Last != NULL && tokenQueue->Last->type != EOL) {
        saveTokenAndReset(tokenQueue, EOL, "", state, positionInLine++);
    }
    saveTokenAndReset(tokenQueue, EoF, "", state, positionInLine);
}

int is_escape_sequence(char previous, char received)    {
    int possible_seq_cnt = 6;
    char possibilities[] ={'"', '\'', 'n', 't','\\', 'x'};
    if (previous == '\\')   {
        for(int i = 0; i < possible_seq_cnt; i++)  {
            if (possibilities[i] == received)
                return 1;
        }
    }
    return 0;
}

char *handleEscapeSequence(char receivedChar, char *buffer) {
    int fgt = 2;
    if (receivedChar == '\\') {
        sliceString(buffer, strlen(buffer)- 1);
    } else if (receivedChar == 'n') {
        sliceString(buffer, strlen(buffer) - fgt);
        strcat(buffer, "\n");
    } else if (receivedChar == 't') {
        sliceString(buffer, strlen(buffer) - fgt);
        strcat(buffer, "\t");
    } else if (receivedChar == 's') {
        sliceString(buffer, strlen(buffer) - fgt);
        strcat(buffer, " ");
    } else if (receivedChar == '\'') {
        sliceString(buffer, strlen(buffer) - fgt);
        strcat(buffer, "\'");
    } else if (receivedChar == 'x') {
        char input[3] = {0, 0, 0};
        input[0] = (char) fgetc(inputFile);
        char exp[3] = {0, 0, 0};
        if (isHexNum(input[0])) {
            exp[0] = input[0];
            input[1] = (char) fgetc(inputFile);
            if (isHexNum(input[1])) {   //both nums are hexdec
                exp[1] = input[1];
            } else
                ungetc(input[1], inputFile);

            int resInt = strtol(exp, NULL, 16);
            char resChar[12];   //every integer should fit in this
            sliceString(buffer, strlen(buffer) - 2); //slice away '\x'
            sprintf(resChar, "%c", resInt);
            strcat(buffer, resChar);
        } else {
            errLex();
        }
    }
    return buffer;
}

int isViableChar(char c) {
    bool res = (c == '+' || c == '-' || c == '*' || c == '/' || c == '!' || c == '?' || c == '=') || c == '.';
    return (isDigit(c) || isDelimiter(c) || isSpecialChar(c) || isWhitespace(c) || res || isLetter(c));
}

int isHexNum(char c) {
    return (isDigit(c) || (c >= 97 && c <= 102) || (c >= 65 && c <= 70));
}

int isBlockComment(char *buffer) {
    if ((strcmp(buffer, "\"\"\"") == 0) || (strcmp(buffer, "\"\"\"\t") == 0) || strcmp(buffer, "\"\"\"\n") == 0) {
        return 1;
    } else if (strlen(buffer) > 3) {
        for (int i = strlen(buffer) - 3; i <= strlen(buffer) - 1; i++) {
            if (buffer[i] != '\"')
                return 0;
        }
        return 1;
    }
    return 0;
}

int isDelimiter(char c) {
    for (int i = 0; i < NUMBER_OF_DELIMITERS; i++) {
        if (c == delimiters[i])
            return 1;
    }
    return 0;
}

int isDigit(char c) {
    if (c >= '0' && c <= '9')
        return 1;

    return 0;
}

int isKeyword(char *word) {
    for (unsigned int i = 0; i < NUMBER_OF_KEYWORDS; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isLetter(char c) {
    int upper = isUppercaseLetter(c);
    int lower = isLowercaseLetter(c);
    return upper || lower;
}

int isLowercaseLetter(char c) {
    if (c >= 'a' && c <= 'z')
        return 1;

    return 0;
}

int isUppercaseLetter(char c) {
    if (c >= 'A' && c <= 'Z')
        return 1;

    return 0;
}

int isOperator(char *word) {
    for (int i = 0; i < NUMBER_OF_OPERATORS; i++) {
        if (strcmp(word, operators[i]) == 0)
            return 1;
    }
    return 0;
}

int isSpecialChar(char c) {
    for (int i = 0; i < NUMBER_OF_SPECIAL_CHARS; i++) {
        if (c == specialChars[i])
            return 1;
    }
    return 0;
}

int isWhitespace(char c) {
    if (c == ' ' || c == '	' || c == '\n') {
        return 1;
    }

    return 0;
}


// SEM MUSIMDOSTAT handleescapesequence()
void runToEndOfDocString(char *buffer) {
    char previousChar = '\0';
    char receivedChar = '\0';
    int cnt = 1;
    while(1)    {
        previousChar = receivedChar;
        receivedChar = (char) fgetc(inputFile);
        if (is_escape_sequence(previousChar, receivedChar))
            handleEscapeSequence(receivedChar, buffer);
        else
            appendToString(receivedChar,buffer, STATE_DOCSTRING);
        if(receivedChar != '"')
            cnt = 0;
        else
            cnt++;
        if(cnt == 3)
            break;
    }
        // run to next "
        /*while ((receivedChar = (char) fgetc(inputFile)) != '"') {
            appendToString(receivedChar, buffer, STATE_DOCSTRING);
        }
         appendToString(receivedChar, buffer, STATE_DOCSTRING);
        int cnt = 1;
        while (1) {
            previousChar = receivedChar;
            receivedChar = (char) fgetc(inputFile);
            appendToString(receivedChar, buffer, STATE_DOCSTRING);
            if (previousChar == '\\')
                handleEscapeSequence(receivedChar, buffer);
            if (receivedChar != '"')
                cnt = 0;
            else
                cnt++;
            if (cnt == 3) {
                found = 1;
                break;
            }
        }*/
}

// takes care of indentation at the beginning of input's line
// returns how many tokens have been added to the queue so that positionInLine reflects change
int handleIndentation(char receivedChar, tDLList *queue) {
    int indentationLevel = 0;
    int res = 0;
    if (receivedChar == '#')
        return -1;
    if (receivedChar == '"')
        return -2;
    if (receivedChar != ' ') {
        res += generateDEDENTTokens(queue, 0);
    } else {
        indentationLevel++;
        while ((receivedChar = (char) fgetc(inputFile)) == ' ') {
            indentationLevel++;
        }
        if (receivedChar == '#')
            return -1;
        else if (receivedChar == '"') {
            return -2;
        } else
            ungetc(receivedChar, inputFile);

        res += generateIndentationTokens(indentationLevel, queue);
    }
    return res;
}

int generateIndentationTokens(int i, tDLList *queue) {
    int res = 0;
    if (top(indentationStack) > i) {
        res += generateDEDENTTokens(queue, i);
    } else if (top(indentationStack) == i)
        return 0;
    else { // i > top(stack)
        res++;
        saveTokenAndReset(queue, INDENT, "", NULL, res);
        push(&indentationStack, i);
    }
    return res;
}

int generateDEDENTTokens(tDLList *queue, int i) {
    int popped = -1;
    int res = 0;
    while ((popped = top(indentationStack)) > i) {
        pop(&indentationStack);
        res++;
        saveTokenAndReset(queue, DEDENT, "", NULL, res);
        if (popped == 0)
            errSyn();
    }
    if (i != top(indentationStack))
        errSyn();
    return res;
}

// -------------------- SCANNER'S MAIN METHOD -------------------- //

// Takes a line in program, does a lexical check and turns it into tokens
// The input argument is modified!
void getLineOfTokens(tDLList *tokenQueue) {
    DLDisposeList(tokenQueue);
    char receivedChar;
    char previousChar = '\0';
    char buffer[MAX_TOKEN_LEN] = "";
    enum stateMachineStates currentState = STATE_START;
    int positionInLine = 0;
    int handledIndentation = 0;
    int blockCommentFlag = 0;

    while ((receivedChar = (char) fgetc(inputFile)) != EOF) {
        if (receivedChar == '\r')
            continue;
        if (!isViableChar(receivedChar) && currentState != STATE_STRING
            && previousChar == '"' && currentState != STATE_DOCSTRING
            && currentState != STATE_LINE_COMMENT) {
            errLex();
        }
        appendToString(receivedChar, buffer, currentState);

        switch (currentState) {
            case STATE_START:   // start of state machine
                if (handledIndentation == 0) {
                    positionInLine = handleIndentation(receivedChar, tokenQueue);
                    if (positionInLine == -1) // in case there's comment first
                        currentState = STATE_LINE_COMMENT;
                    if (positionInLine == -2) { // in case it's documentationString
                        currentState = STATE_DOCSTRING;
                        blockCommentFlag = 1;
                        appendToString('"', buffer, currentState);
                        appendToString('"', buffer, currentState);
                    }
                    handledIndentation++;
                    positionInLine = 0;
                }
                if (receivedChar == '\'') {
                    currentState = STATE_STRING;
                } else if (isSpecialChar(receivedChar)) {
                    saveTokenAndReset(tokenQueue, specialChar, buffer, &currentState, positionInLine++);
                } else if (isLetter(receivedChar)) {
                    currentState = STATE_KW_ID;
                } else if (receivedChar == '_') {
                    currentState = STATE_ID;
                } else if (isDigit(receivedChar)) {
                    currentState = STATE_NUMBER;
                } else if (receivedChar == '#') {
                    currentState = STATE_LINE_COMMENT;
                } else if (receivedChar == '\"') {
                    currentState = STATE_DOCSTRING;
                } else if (isOperator(buffer) || receivedChar == '!') {
                    currentState = STATE_OPERATOR;
                } else if (!isWhitespace(receivedChar)) {
                    currentState = STATE_ERROR;
                }
                break;

            case STATE_LINE_COMMENT:    // state one-line for comment
                if (receivedChar == '\n') {
                    currentState = STATE_START;
                    strcpy(buffer, "");
                }
                break;

            case STATE_KW_ID:   // state for keyword or identifier
                if (receivedChar == '_' || isDigit(receivedChar)) {
                    currentState = STATE_ID;
                } else if (isKeyword(buffer)) {
                    currentState = STATE_KEYWORD;
                } else if (isWhitespace(receivedChar)) {
                    saveTokenAndReset(tokenQueue, identifier, buffer, &currentState, positionInLine++);
                } else if ((!isLetter(receivedChar) && !isDigit(receivedChar)) || isDelimiter(receivedChar)) {
                    returnLastCharToInput(buffer, receivedChar);
                    saveTokenAndReset(tokenQueue, identifier, buffer, &currentState, positionInLine++);
                }
                break;

            case STATE_ID:  // state for identifier
                if (isDelimiter(receivedChar)) {
                    saveTokenAndReset(tokenQueue, identifier, buffer, &currentState, positionInLine++);
                } else if ((!isDigit(receivedChar) && !isLetter(receivedChar))
                           || isSpecialChar(receivedChar)) {

                    returnLastCharToInput(buffer, receivedChar);
                    saveTokenAndReset(tokenQueue, identifier, buffer, &currentState, positionInLine++);
                }
                break;

            case STATE_NUMBER:  // state for number (not yet decided what kind)
                if (strlen(buffer) == 2 && previousChar == '0' && receivedChar == '0') {
                    currentState = STATE_ERROR;
                    continue;
                } else if (receivedChar == '.') {
                    currentState = STATE_DECIMAL_NUM;
                } else if (receivedChar == 'e' || receivedChar == 'E') {
                    currentState = STATE_EXP_NUM;
                } else if (isDelimiter(receivedChar)) {
                    saveTokenAndReset(tokenQueue, integer, buffer, &currentState, positionInLine++);
                }
                else if (isSpecialChar(receivedChar)) {
                    returnLastCharToInput(buffer, receivedChar);
                    saveTokenAndReset(tokenQueue, integer, buffer, &currentState, positionInLine++);
                }
                else if (!isDigit(receivedChar)) {
                    currentState = STATE_ERROR;
                }
                break;

            case STATE_DECIMAL_NUM: // state for decimal numbers
                if ((previousChar == '.') && (!isDigit(receivedChar))) {
                    currentState = STATE_ERROR;
                    continue;
                } else if (receivedChar == 'e' || receivedChar == 'E') {
                    currentState = STATE_EXP_NUM;
                } else if ((previousChar == 'e' || previousChar == 'E')
                           && (isDigit(receivedChar) || receivedChar == '+' || receivedChar == '-')) {
                    currentState = STATE_EXP_NUM;
                } else if (receivedChar == '.') {
                    currentState = STATE_ERROR;
                    continue;
                } else if (isDelimiter(receivedChar)) {
                    saveTokenAndReset(tokenQueue, floatingPoint, buffer, &currentState, positionInLine++);
                } else if (!isDigit(receivedChar)) {
                    returnLastCharToInput(buffer, receivedChar);
                    saveTokenAndReset(tokenQueue, floatingPoint, buffer, &currentState, positionInLine++);
                }
                break;

            case STATE_EXP_NUM: // state for exponential numbers
                if (isDuplicateSignInNum(buffer) || receivedChar == '.') {
                    currentState = STATE_ERROR;
                    continue;
                } else if (receivedChar == '0' && (previousChar == 'e' || previousChar == 'E')) {
                    if (!checkExponent(buffer)) {
                        currentState = STATE_ERROR;
                        continue;
                    }
                } else if (isDelimiter(receivedChar)) {
                    if (!checkExponent(buffer)) {
                        currentState = STATE_ERROR;
                        continue;
                    }
                    if (previousChar != 'e' && previousChar != 'E') {
                        saveTokenAndReset(tokenQueue, floatingPoint, buffer, &currentState, positionInLine++);
                    }
                } else if ((!isDigit(receivedChar)) && receivedChar != '+' && receivedChar != '-'
                           && previousChar != 'e' && previousChar != 'E') {
                    returnLastCharToInput(buffer, receivedChar);
                    if (!checkExponent(buffer)) {
                        currentState = STATE_ERROR;
                        continue;
                    }
                    saveTokenAndReset(tokenQueue, floatingPoint, buffer, &currentState, positionInLine++);
                }
                break;

            case STATE_KEYWORD: // state for keyword
                if (isDelimiter(receivedChar)) {
                    saveTokenAndReset(tokenQueue, keyword, buffer, &currentState, positionInLine++);
                } else if ((!isLetter(receivedChar) && !isDigit(receivedChar)) && receivedChar != '_') {
                    returnLastCharToInput(buffer, receivedChar);
                    saveTokenAndReset(tokenQueue, keyword, buffer, &currentState, positionInLine++);
                } else {
                    currentState = STATE_KW_ID;
                }
                break;

            case STATE_OPERATOR:    // state for operator
                if (receivedChar != '=' && previousChar == '!') {
                    if (!isWhitespace(receivedChar))
                        returnLastCharToInput(buffer, receivedChar);
                    currentState = STATE_ERROR;
                } else {
                    if ((receivedChar != '/' && receivedChar != '=' && !isWhitespace(receivedChar)) ||
                        strlen(buffer) > 2) {
                        returnLastCharToInput(buffer, receivedChar);
                    }
                    saveTokenAndReset(tokenQueue, Operator, buffer, &currentState, positionInLine++);
                }
            case STATE_STRING:  // state for string
                if ((receivedChar == '\'') && (previousChar != '\\')) {
                    for(int i = 0; i < strlen(buffer); i++) {
                        buffer[i] = buffer[i+1];
                        if (buffer[i] == '\0')
                            buffer[i - 1] = '\0';
                    }
                    saveTokenAndReset(tokenQueue, string, buffer, &currentState, positionInLine++);
                }
                if (previousChar == '\\') {       // in case there's escape sequence
                    handleEscapeSequence(receivedChar, buffer);
                    receivedChar = 0;
                }
                break;

            case STATE_ERROR:   // state for tokens that violate any lexical rule
                if (isDelimiter(receivedChar)) {
                    saveTokenAndReset(tokenQueue, error, buffer, &currentState, positionInLine++);
                }
                errLex();
                break;

            case STATE_DOCSTRING:   // state for documentation string
                if (blockCommentFlag == 1) {   //reading through a comment
                    runToEndOfDocString(buffer);
                    if (isBlockComment(buffer)) {    //found end
                        blockCommentFlag = 0;
                        //sliceString(buffer, strlen(buffer) - 3); - changed 27.10.
                        saveTokenAndReset(tokenQueue, docString, buffer, &currentState, positionInLine++);
                    }
                } else {
                    blockCommentFlag = isBlockComment(buffer); //outside of a comment
                    if (blockCommentFlag) {
                        //strcpy(buffer, ""); - changed 27.10.2019
                    }
                }
                break;

        }   // end of main switch

        if (receivedChar == '\n' && currentState != STATE_DOCSTRING) {
            saveTokenAndReset(tokenQueue, EOL, buffer, &currentState, positionInLine++);
            if (SCAN_WHOLE_FILE) {   // by default, this should be 0
                positionInLine = 0;
                previousChar = '\0';
                handledIndentation = 0;
            } else
                return;
        }
        previousChar = receivedChar;
    }
    handleEOF(tokenQueue, buffer, &currentState, positionInLine);
    tokenQueue->Act = tokenQueue->First;
}





