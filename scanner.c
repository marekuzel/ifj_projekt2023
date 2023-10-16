#include "scanner.h"
#include "errors.h"

void initialize_token(token_t* token) {
    if (token != NULL) {
        token->type = TOKEN_ZERO;
        token->value = NULL;
        token->next = NULL;
    }
}

void clear_buffer(char* buffer) {
    buffer[0] = '\0';
}

bool check_for_keyword(char* text) {
    for(int i = 0; i < NOF_KEYWORDS; i++) {
        if (strcmp(keywords[i], text) == 0) {
            return true;
        }
    }
    return false;
}

bool check_for_datatype(char* text) {
    for(int i = 0; i < NOF_DATATYPES; i++) {
        if (strcmp(keywords[i], text) == 0) {
            return true;
        }
    }
    return false;
}
    
token_t* generate_token() {
    FILE* stream = stdin;
    token_t* token = (token_t*)malloc(sizeof(struct tok)); 
    ScannerState state = STATE_START;
    char* buffer = malloc(sizeof(char) * MAX_STRLEN); // TREBA POTOM UVOLNIT V PARSERU !!
    clear_buffer(buffer);
    int buffer_index = 1;
    int multiline_string_counter = 0;
    bool multiline_string_ok = false;
    // V pripade ze chci vratit charakter do stdin:
    // ungetc(ch, stream);

    initialize_token(token);

    int ch;
    while (true) {
        ch = fgetc(stream);
        switch (state) {
            case STATE_START:
                if (isalpha(ch) || ch == '_') {
                    state = STATE_TEXT;
                    sprintf(buffer, "%c", ch);
                }
                else if (isspace(ch)) {
                    continue;
                }
                else if (ch == '/') {
                    state = STATE_SLASH;
                    sprintf(buffer, "%c", ch);
                }
                else if (ch == '"') {
                    state = STATE_STRING;
                }
                else if (ch == EOF) {
                    token->type = TOKEN_EOF;
                    return token;
                }
                // ..
                break;

            case STATE_TEXT:
                if (isalpha(ch) || ch == '_' || isdigit(ch)) {
                    buffer[buffer_index++] = ch;
                } else {
                    if (check_for_keyword(buffer)) {
                        token->type = TOKEN_KEYWORD;
                    } else {
                        token->type = TOKEN_IDENTIFIER;
                    }

                    token->value = buffer;
                    return token;
                }
                break;

            case STATE_SLASH:
                if (ch == '/') {
                    state = STATE_LINE_COMMENT;
                    clear_buffer(buffer);
                }
                else if (ch == '*') {
                    state = STATE_BLOCK_COMMENT;
                    clear_buffer(buffer);
                }
                else {
                    token->type = TOKEN_TERM;
                    token->value = buffer;
                    return token;
                }

                break;

            case STATE_LINE_COMMENT:
                if (ch == '\n') {
                    state = STATE_START;
                }

                break;

            case STATE_BLOCK_COMMENT:
                if (ch == '*') {
                    state = STATE_END_BLOCK_COMMENT;
                }

                break;

            case STATE_END_BLOCK_COMMENT:
                if (ch == '/') {
                    state = STATE_START;
                } else {
                    state = STATE_BLOCK_COMMENT;
                }

                break;

            case STATE_STRING:
                if (ch == '"') {
                    if (strlen(buffer) == 0) {
                        state = STATE_TWO_DOUBLE_QUOTES;
                        break;
                    } else {
                        token->type = TOKEN_STRING;
                        token->value = buffer;
                        return token;
                    }
                }

                if (strlen(buffer) > 0) {
                    buffer[buffer_index++] = ch;
                } else {
                    sprintf(buffer, "%c", ch);
                }

                break;

            case STATE_TWO_DOUBLE_QUOTES:
                if (ch == '"') {
                    state = STATE_MULTILINE_STRING;
                } else {
                    ungetc(ch, stream);
                    token->type = TOKEN_STRING;
                    token->value = buffer;
                    return token;
                }

                break;

            case STATE_MULTILINE_STRING:
                if (ch == '"') {
                    if (++multiline_string_counter >= 3) {
                        if (buffer[strlen(buffer)-3] != '\n') {
                            free(token); free(buffer);
                            fprintf(stderr, "Lexical error encountered.\n");
                            exit(LEXICAL_ERROR);
                        }

                        buffer[strlen(buffer)-3] = '\0';

                        token->type = TOKEN_STRING;
                        token->value = buffer;
                        return token;
                    }
                } else {
                    multiline_string_counter = 0;
                    if (!multiline_string_ok) {
                        if (ch != '\n') {
                            free(token); free(buffer);
                            fprintf(stderr, "Lexical error encountered.\n");
                            exit(LEXICAL_ERROR);
                        } else {
                            multiline_string_ok = true;
                            break;
                        }
                    } 

                }
                
                if (strlen(buffer) > 0) {
                    buffer[buffer_index++] = ch;
                } else {
                    sprintf(buffer, "%c", ch);
                }

                break;
        }
    }
}
