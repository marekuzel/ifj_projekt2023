/**
 * Project: Compliler IFJ23 implementation 
 * File: scanner.h
 * 
 * @brief interface of lexical analysis
 * 
 * @author Markéte Belatková xbelat02
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>
#include "errors.h"
#include "utils.h"

#ifndef SCANNER_H
#define SCANNER_H

#define SCANNER_ERROR(msg) fprintf(stderr,"%s.\n",msg); exit(LEXICAL_ERROR);

typedef enum fsm_state_e {
    STATE_START,
    STATE_TEXT,
    STATE_SLASH,
    STATE_LINE_COMMENT,
    STATE_BLOCK_COMMENT,
    STATE_END_BLOCK_COMMENT,
    STATE_STRING,
    STATE_MULTILINE_STRING_START,
    STATE_MULTILINE_STRING,
    STATE_TWO_DOUBLE_QUOTES,
    STATE_EQUALS,
    STATE_QUESTION,
    STATE_OPERATOR,
    STATE_NUMBER,
    STATE_DECIMAL,
    STATE_DECIMAL_POINT, 
    STATE_EXPONENT,
    STATE_DASH, 
    STATE_ESCAPE_SEQUENCE,
    STATE_UNDERSCORE
} ScannerState;

#define NOF_KEYWORDS 8
#define NOF_DATATYPES 3
#define MAX_DTT_KWD_LEN 10

/**
 * @brief function checking if returning token matches with some of keywords
 * @param text: checked value of returning token
 * @return true if value of token matches with some of keywords
 * @return false otherwise
 */
bool check_for_keyword(char* text);

/**
 * @brief function checking if returning token matches with some of datatypes
 * @param text: checked value of returning token
 * @return true if value of token matches with some of datatypes
 * @return false otherwise
 */
bool check_for_datatype(char* text);

/**
 * @brief function searching for keywords' token_type matches with token value
 * @param keyword: value of returning token
 * @return token type matching with right keyword
 * @return token type TOKEN_ZERO otherwise
 */
TokenType keyword_to_token(char* keyword);

/**
 * @brief function searching for datatypes' token_type matches with token value
 * @param datatype: value of returning token
 * @return token type matching with right datatype
 * @return token type TOKEN_ZERO otherwise
 */
TokenType datatype_to_token(char* datatype);

/**
 * @brief function checking type and validity of escape sequence
 * @param buffer: buffer of characters corresponding to the token value
 * @param ch: character specifying the type of escape sequence
 * @return STATE_START if invalid escape sequence was given or if sequence appending to buffer failed
 * @return STATE_ESCAPE if sequence "\u" was given
 * @return STATE_STRING otherwise
 */
ScannerState handle_escape_sequence(BufferT* buffer, char ch);

/**
 * @brief function generating token and checking for lexical errors based on Final State Machine
 * @return token structure with token type and value, terminate with error otherwise
 */
TokenT *generate_token();

#endif