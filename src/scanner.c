#include "scanner.h"

char keywords[NOF_KEYWORDS][MAX_DTT_KWD_LEN] = {
    "else", "func", "if", "let", "nil", "return", "var", "while"
};

char datatypes[NOF_DATATYPES][MAX_DTT_KWD_LEN] = {
    "Double", "Int", "String"
};

void append_and_check(BufferT *buffer, const char ch) {
    if (buffer_append(buffer, ch) != BUFF_APPEND_SUCCES) {
        fprintf(stderr, "Internal compiler error. \n");
        exit(INTERNAL_COMPILER_ERROR);
    }
}

void error_exit(TokenT *token, BufferT *buffer, char* message, int exit_code) {
    token_dtor(token); // free(token) bude v token_dtor 
    buffer_detor(buffer);
    fprintf(stderr, "%s.\n", message);
    exit(exit_code);
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
        if (strcmp(datatypes[i], text) == 0) {
            return true;
        }
    }
    return false;
}

TokenType keyword_to_token(char* keyword) {
    if (strcmp(keyword, "else") == 0) {
        return TOKEN_ELSE;
    }
    else if (strcmp(keyword, "func") == 0) {
        return TOKEN_FUNC;
    }
    else if (strcmp(keyword, "if") == 0) {
        return TOKEN_IF;
    }
    else if (strcmp(keyword, "let") == 0) {
        return TOKEN_LET;
    }
    else if (strcmp(keyword, "nil") == 0) {
        return TOKEN_NIL;
    }
    else if (strcmp(keyword, "return") == 0) {
        return TOKEN_RETURN;
    }
    else if (strcmp(keyword, "var") == 0) {
        return TOKEN_VAR;
    }
    else if (strcmp(keyword, "while") == 0) {
        return TOKEN_WHILE;
    } 
    else {
        return TOKEN_ZERO;
    }
}

TokenType datatype_to_token(BufferT* buffer, int ch) {
    if (ch == '?') {
        append_and_check(buffer, ch);
    }
    if (strcmp(buffer->bytes, "Double") == 0) {
        return TOKEN_DT_DOUBLE;
    }
    else if (strcmp(buffer->bytes, "Double?") == 0) {
        return TOKEN_DT_DOUBLE_NIL;
    }
    else if (strcmp(buffer->bytes, "Int") == 0) {
        return TOKEN_DT_INT;
    }
    else if (strcmp(buffer->bytes, "Int?") == 0) {
        return TOKEN_DT_INT_NIL;
    }
    else if (strcmp(buffer->bytes, "String") == 0) {
        return TOKEN_DT_STRING;
    } 
    else if (strcmp(buffer->bytes, "String?") == 0) {
        return TOKEN_DT_DOUBLE_NIL;
    }
    else {
        return TOKEN_ZERO;
    }
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
    bool escape_next = false;
    bool exp_sign = false;
    bool empty_exp = true;
    // V pripade ze chci vratit charakter do stdin:
    // ungetc(ch, stream);

    int ch;
    while (true) {
        ch = fgetc(stream);

        switch (state) {
            case STATE_START:
                if (isalpha(ch) || ch == '_') {
                    state = STATE_TEXT;
                    append_and_check(&buffer, ch);
                }
                else if (isdigit(ch) || ch == '0') {
                    state = STATE_NUMBER;
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
                    return token;
                }
                else if (ch == '+' || ch == '-' || ch == '*') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    return token;
                }
                else if (ch == '!') {
                    state = STATE_EXCLAMATION;
                    append_and_check(&buffer, ch);
                }
                else if (ch == '=') {
                    state = STATE_EQUALS;
                    append_and_check(&buffer, ch);
                }
                else if (ch == '?') {
                    state = STATE_QUESTION;
                    append_and_check(&buffer, ch);
                }
                else if (ch == '<' || ch == '>') {
                    state = STATE_RELATIONAL_OPERATOR;
                    append_and_check(&buffer, ch);
                }
                else if (ch == '(') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_L_BRACKET, &buffer);
                    return token;
                }
                else if (ch == ')') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_R_BRACKET, &buffer);
                    return token;
                }
                else if (ch == ':') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_COLON, &buffer);
                    return token;
                }
                else if (ch == '{') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_LC_BRACKET, &buffer);
                    return token;
                }
                else if (ch == '}') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_RC_BRACKET, &buffer);
                    return token;
                }
                else if (ch == ',') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_COMMA, &buffer);
                    return token;
                }
                // ..
                break;

            case STATE_TEXT:
                if (isalpha(ch) || ch == '_' || isdigit(ch)) {
                    append_and_check(&buffer, ch);
                } else {
                    if (check_for_keyword(buffer.bytes)) {
                        token_init(token, keyword_to_token(buffer.bytes), &buffer);
                        ungetc(ch, stream);
                    } else if(check_for_datatype(buffer.bytes)) {
                        token_init(token, datatype_to_token(&buffer, ch), &buffer);
                    } else {
                        token_init(token, TOKEN_IDENTIFIER, &buffer);
                        ungetc(ch, stream);
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
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    ungetc(ch, stream);
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
                if (escape_next) {
                    switch (ch) {
                        case '"':
                            append_and_check(&buffer, '\"');
                            break;
                        case 'n':
                            append_and_check(&buffer, '\n');
                            break;
                        case 'r':
                            append_and_check(&buffer, '\r');
                            break;
                        case 't':
                            append_and_check(&buffer, '\t');
                            break;
                        case '\\':
                            append_and_check(&buffer, '\\');
                            break;
                        default:
                            error_exit(token, &buffer, "Invalid escape sequence.", LEXICAL_ERROR);
                            break;
                    }
                    escape_next = false;
                    break;
                }
                if (ch == '"') {
                    if (buffer.length == 0) {
                        state = STATE_TWO_DOUBLE_QUOTES;
                        break;
                    } 
                    else {
                        token_init(token, TOKEN_STRING, &buffer);
                        return token;
                    }
                }
                else if (ch == '\\') {
                    escape_next = true;
                    break;
                }
                append_and_check(&buffer, ch);
                break;

            case STATE_TWO_DOUBLE_QUOTES:
                if (ch == '"') {
                    state = STATE_MULTILINE_STRING;
                } 
                else {
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
                        return token;
                    }
                } 
                else {
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
                break;

            case STATE_EXCLAMATION:
                if (ch == '=') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    return token;
                } 
                else {
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    ungetc(ch, stream);
                    return token;
                }
                break;

            case STATE_EQUALS:
                if (ch == '=') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    return token;
                } 
                else {
                    token_init(token, TOKEN_ASSIGN, &buffer);
                    ungetc(ch, stream);
                    return token;
                }
                break;

            case STATE_QUESTION:
                if (ch == '?') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    return token;
                } 
                else {
                    ungetc(ch, stream);
                    return token;
                }
                break;

            case STATE_RELATIONAL_OPERATOR:
                if (ch == '=') {
                    append_and_check(&buffer, ch);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    return token;
                } 
                else {
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    ungetc(ch, stream);
                    return token;
                }
                break;

            case STATE_NUMBER:
                if (ch == '.') {
                    state = STATE_DECIMAL_POINT;
                    append_and_check(&buffer, ch);
                }
                else if (isdigit(ch) || ch == '0') {
                    append_and_check(&buffer, ch);
                } 
                else if (ch == 'e' || ch == 'E') {
                    state = STATE_EXPONENT;
                    append_and_check(&buffer, ch);
                }
                else {
                    token_init(token, TOKEN_INTEGER, &buffer);
                    ungetc(ch, stream);
                    return token;
                }
                break;

            case STATE_DECIMAL_POINT:
                if (!(isdigit(ch) || ch == '0')) {
                    error_exit(token, &buffer, "Invalid decimal number", LEXICAL_ERROR);
                }
                else {
                    state = STATE_DECIMAL;
                    append_and_check(&buffer, ch);
                }
                break;

            case STATE_DECIMAL:
                if (isdigit(ch) || ch == '0') {
                    append_and_check(&buffer, ch);
                }
                else if (ch == 'e' || ch == 'E') {
                    state = STATE_EXPONENT;
                    append_and_check(&buffer, ch);
                }
                else {
                    token_init(token, TOKEN_DOUBLE, &buffer);
                    ungetc(ch, stream);
                    return token;
                }
                break;
       
            case STATE_EXPONENT:
                if ((ch == '+' || ch == '-') && exp_sign == false) {
                    exp_sign = true;
                    append_and_check(&buffer, ch);
                } 
                else if (isdigit(ch) || ch == '0') {
                    empty_exp = false;
                    append_and_check(&buffer, ch);
                }
                else {
                    if (empty_exp) {
                        error_exit(token, &buffer, "Invalid exponent", LEXICAL_ERROR);
                    }
                    token_init(token, TOKEN_DOUBLE, &buffer);
                    ungetc(ch, stream);
                    return token;
                }

        }
    }
}

int main() {
    TokenT* token = generate_token();
    while (token != NULL && token->type != TOKEN_EOF) {
        
        printf("TOKEN T%d VALUE ", token->type);
        print_Token(token);

        // token_dtor(token);
        token = generate_token();
    }
    // printf("TOKEN T%d VALUE [%s]\n", token->type, token->value.str);

    return 0;
}