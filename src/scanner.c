#include "scanner.h"
#include "errors.h"
#include "utils.h"

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

void print_Token(TokenT *token){
    switch(token->type) {
        case TOKEN_INTEGER:
            printf("%d\n",token->value.i);
        case TOKEN_DECIMAL:
            printf("%f\n",token->value.d);
        dafault:
            printf("%s\n",token->value.str);
    }
}

void append_and_check(BufferT *buffer, const char ch) {
    if (buffer_append(buffer, ch) != BUFF_APPEND_SUCCES) {
        fprintf(stderr, "Internal compiler error. \n");
        exit(INTERNAL_COMPILER_ERROR);
    }
    fprintf(stderr, "%s \n", buffer->bytes);
}

void error_exit(TokenT *token, BufferT *buffer, char* message, int exit_code) {
    token_dtor(token); // free(token) bude v token_dtor 
    buffer_detor(buffer);
    fprintf(stderr, "%s.\n", message);
    exit(exit_code);
}
    
TokenT* generate_token() {
    FILE* stream = stdin;
    ScannerState state = STATE_START;
    BufferT buffer;
    if (buffer_init(&buffer) != BUFF_INIT_SUCCES) {
        fprintf(stderr, "Internal compiler error. \n");
        exit(INTERNAL_COMPILER_ERROR);
    }
    TokenT* token = (TokenT*)malloc(sizeof(TokenT)); // uvolnit v parseru
    if (token == NULL) {
        fprintf(stderr, "Internal compiler error. \n");
        exit(INTERNAL_COMPILER_ERROR);
    }

    int multiline_string_counter = 0;
    bool multiline_string_ok = false;
    // V pripade ze chci vratit charakter do stdin:
    // ungetc(ch, stream);

    //token_init(token);

    int ch;
    while (true) {
        ch = fgetc(stream);
        switch (state) {
            case STATE_START:
                if (isalpha(ch) || ch == '_') {
                    state = STATE_TEXT;
                    append_and_check(&buffer, ch);
                }
                else if (isspace(ch)) {
                    continue;
                }
                else if (ch == '/') {
                    state = STATE_SLASH;
                     append_and_check(&buffer, ch);
                }
                else if (ch == '"') {
                    state = STATE_STRING;
                }
                else if (ch == EOF) {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_EOF, &buffer);

                    print_Token(token);
                    fprintf(stderr, "TOKEN T%d VALUE %s  (eof) \n", token->type, token->value.str);

                    return token;
                }
                // ..
                break;

            case STATE_TEXT:
                if (isalpha(ch) || ch == '_' || isdigit(ch)) {
                    append_and_check(&buffer, ch);
                } else {
                    if (check_for_keyword(buffer.bytes)) {
                        token_init(token, TOKEN_KEYWORD, &buffer);
                        print_Token(token);
                    } else if(check_for_datatype(buffer.bytes)) {
                        token_init(token, TOKEN_DATATYPE, &buffer);
                    } else {
                        token_init(token, TOKEN_IDENTIFIER, &buffer);
                        print_Token(token);
                    }

                    return token;
                }
                break;

            case STATE_SLASH:
                if (ch == '/') {
                    state = STATE_LINE_COMMENT;
                    buffer_clear(&buffer);
                }
                else if (ch == '*') {
                    state = STATE_BLOCK_COMMENT;
                    buffer_clear(&buffer);
                }
                else {
                    token_init(token, TOKEN_TERM, &buffer);
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
                    if (buffer.length == 0) {
                        state = STATE_TWO_DOUBLE_QUOTES;
                        break;
                    } else {
                        token_init(token, TOKEN_STRING, &buffer);
                        return token;
                    }
                }
                append_and_check(&buffer, ch);

                break;

            case STATE_TWO_DOUBLE_QUOTES:
                if (ch == '"') {
                    state = STATE_MULTILINE_STRING;
                } else {
                    ungetc(ch, stream);
                    token_init(token, TOKEN_STRING, &buffer);
                    return token;
                }

                break;

            case STATE_MULTILINE_STRING:
                if (ch == '"') {
                    if (++multiline_string_counter >= 3) {
                        if (buffer.bytes[buffer.length-3] != '\n') {
                            error_exit(token, &buffer, "Lexical error", LEXICAL_ERROR);
                        }

                        buffer.bytes[buffer.length-3] = '\0';

                        token_init(token, TOKEN_STRING, &buffer);
                        // token->type = TOKEN_STRING;
                        // token->value = buffer;
                        return token;
                    }
                } else {
                    multiline_string_counter = 0;
                    if (!multiline_string_ok) {
                        if (ch != '\n') {
                            error_exit(token, &buffer, "Lexical error", LEXICAL_ERROR);
                        } else {
                            multiline_string_ok = true;
                            break;
                        }
                    } 

                }
                
                append_and_check(&buffer, ch);
                // if (buffer.length > 0) {
                //     buffer[buffer_index++] = ch;
                // } else {
                //     sprintf(buffer, "%c", ch);
                // }

                break;
        }
    }
}

int main() {
    TokenT* token = NULL;
    while (true) {
        token = generate_token();
        if (token->type == TOKEN_EOF) {
            break;
        }

        printf("TOKEN T%d VALUE %s\n", token->type, token->value.str);

        token_dtor(token);
        free(token);
    }

    return 0;
}