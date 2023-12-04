#include "scanner.h"

char keywords[NOF_KEYWORDS][MAX_DTT_KWD_LEN] = {
    "else", "func", "if", "let", "nil", "return", "var", "while"
};

char datatypes[NOF_DATATYPES][MAX_DTT_KWD_LEN] = {
    "Double", "Int", "String"
};


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

TokenType datatype_to_token(char* datatype) {
    if (strcmp(datatype, "Double") == 0) {
        return TOKEN_DT_DOUBLE;
    }
    else if (strcmp(datatype, "Double?") == 0) {
        return TOKEN_DT_DOUBLE_NIL;
    }
    else if (strcmp(datatype, "Int") == 0) {
        return TOKEN_DT_INT;
    }
    else if (strcmp(datatype, "Int?") == 0) {
        return TOKEN_DT_INT_NIL;
    }
    else if (strcmp(datatype, "String") == 0) {
        return TOKEN_DT_STRING;
    } 
    else if (strcmp(datatype, "String?") == 0) {
        return TOKEN_DT_STRING_NIL;
    }
    else {
        return TOKEN_ZERO;
    }
}

ScannerState handle_escape_sequence(BufferT* buffer, char ch) {
    buff_ret_t hex_ret;
    switch (ch) {
        case '"':
            hex_ret = buffer_apend_hex_num(buffer, "22");
            break;
        case 'n':
            hex_ret = buffer_apend_hex_num(buffer, "a");
            break;
        case 'r':
            hex_ret = buffer_apend_hex_num(buffer, "d");
            break;
        case 't':
            hex_ret = buffer_apend_hex_num(buffer, "9");
            break;
        case '\\':
            hex_ret = buffer_apend_hex_num(buffer, "5c");
            break;
        case 'u':
            return STATE_ESCAPE_SEQUENCE;
            break;
        default:
            return STATE_START;
            break;
    }
    if (hex_ret != BUFF_NUM_CVT_SUCCES) {
        return STATE_START;
    }
    return STATE_STRING;
}
    
TokenT* generate_token() {
    FILE* stream = stdin;
    ScannerState state = STATE_START;
    BufferT buffer;
    
    buffer_init(&buffer);

    TokenT* token = (TokenT*)calloc(1,sizeof(TokenT)); // uvolnit v parseru

    CHECK_MEM_ERR(token)

    int multiline_string_counter = 0;
    bool multiline_string_ok = false;
    bool escape_next = false;
    bool exp_sign = false;
    bool empty_exp = true;
    bool multiline_mode = false;
    char number_buffer[3];
    int esc_sqv_cnt = 0;
    // V pripade ze chci vratit charakter do stdin:
    // ungetc(ch, stream);

    int ch;
    while (true) {
        ch = fgetc(stream);
        if (ch == EOF && state != STATE_START) {
            SCANNER_ERROR("Unterminated comment or string")
        }

        switch (state) {
            case STATE_START:
                if (isalpha(ch) || ch == '_') {
                    state = STATE_TEXT;
                    buffer_append(&buffer, ch);
                }
                else if (isdigit(ch) || ch == '0') {
                    state = STATE_NUMBER;
                    buffer_append(&buffer, ch);
                }
                else if (isspace(ch)) {
                    continue;
                }
                else if (ch == '/') {
                    state = STATE_SLASH;
                    buffer_append(&buffer, ch);
                }
                else if (ch == '"') {
                    state = STATE_STRING;
                }
                else if (ch == EOF) {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_EOF, &buffer);
                    return token;
                }
                else if (ch == '+' || ch == '*') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    return token;
                }
                else if (ch == '-') {
                    buffer_append(&buffer, ch);
                    state = STATE_DASH;
                }
                else if (ch == '!' || ch == '<' || ch == '>') {
                    state = STATE_OPERATOR;
                    buffer_append(&buffer, ch);
                }
                else if (ch == '=') {
                    state = STATE_EQUALS;
                    buffer_append(&buffer, ch);
                }
                else if (ch == '?') {
                    state = STATE_QUESTION;
                    buffer_append(&buffer, ch);
                }
                else if (ch == '(') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_L_BRACKET, &buffer);
                    return token;
                }
                else if (ch == ')') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_R_BRACKET, &buffer);
                    return token;
                }
                else if (ch == ':') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_COLON, &buffer);
                    return token;
                }
                else if (ch == '{') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_LC_BRACKET, &buffer);
                    return token;
                }
                else if (ch == '}') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_RC_BRACKET, &buffer);
                    return token;
                }
                else if (ch == ',') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_COMMA, &buffer);
                    return token;
                }
                else {
                    SCANNER_ERROR("Unexpected symbol")
                }
                break;

            case STATE_TEXT:
                if (isalpha(ch) || ch == '_' || isdigit(ch)) {
                    buffer_append(&buffer, ch);
                } else {
                    if (check_for_keyword(buffer.bytes)) {
                        token_init(token, keyword_to_token(buffer.bytes), &buffer);
                        ungetc(ch, stream);
                    } else if(check_for_datatype(buffer.bytes)) {
                        if (ch == '?') {
                            buffer_append(&buffer, ch);
                        } else {
                            ungetc(ch, stream);
                        }
                        token_init(token, datatype_to_token(buffer.bytes), &buffer);
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
                    buffer_append(&buffer, '\\');
                    state = handle_escape_sequence(&buffer, ch);
                    if (state == 0) { 
                        SCANNER_ERROR("Invalid escape sequence")
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
                buffer_append(&buffer, ch);
                break;

            case STATE_TWO_DOUBLE_QUOTES:
                if (ch == '"') {
                    state = STATE_MULTILINE_STRING;
                    multiline_mode = true;
                } 
                else {
                    ungetc(ch, stream);
                    token_init(token, TOKEN_STRING, &buffer);
                    return token;
                }
                break;

            case STATE_MULTILINE_STRING:
                if (escape_next) {
                    buffer_append(&buffer, '\\');
                    state = handle_escape_sequence(&buffer, ch);
                    if (state == 0) {
                        SCANNER_ERROR("Invalid escape sequence")
                    }
                    if (state == STATE_STRING) {
                        state = STATE_MULTILINE_STRING;
                    }
                    
                    escape_next = false;
                    break;
                }
                else if (ch == '"') {
                    if (++multiline_string_counter >= 3) {
                        if (buffer.bytes[buffer.length-3] != '\n') {
                            SCANNER_ERROR("Lexical error")
                        }
                        buffer.bytes[buffer.length-3] = '\0';
                        token_init(token, TOKEN_STRING, &buffer);
                        return token;
                    }
                } 
                else if (ch == '\\') {
                    escape_next = true;
                    break;
                }
                else {
                    multiline_string_counter = 0;
                    if (!multiline_string_ok) {
                        if (ch != '\n') {
                            SCANNER_ERROR("Lexical error - invalid multiline string")
                        } else {
                            multiline_string_ok = true;
                            break;
                        }
                    } 
                }
                buffer_append(&buffer, ch);
                break;

            case STATE_DASH:
                if (ch == '>') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_ARROW, &buffer);
                }
                else {
                    ungetc(ch, stream);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                }
                return token;

            case STATE_OPERATOR:
                if (ch == '=') {
                    buffer_append(&buffer, ch);
                } 
                else {
                    ungetc(ch, stream);
                }
                token_init(token, TOKEN_OPERATOR, &buffer);
                return token;

            case STATE_EQUALS:
                if (ch == '=') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                } 
                else {
                    token_init(token, TOKEN_ASSIGN, &buffer);
                    ungetc(ch, stream);
                }
                return token;

            case STATE_QUESTION:
                if (ch == '?') {
                    buffer_append(&buffer, ch);
                    token_init(token, TOKEN_OPERATOR, &buffer);
                    return token;
                } 
                else {
                    SCANNER_ERROR("Lexical error - unexpected question mark")
                }
                break;

            case STATE_NUMBER:
                if (ch == '.') {
                    state = STATE_DECIMAL_POINT;
                    buffer_append(&buffer, ch);
                }
                else if (isdigit(ch) || ch == '0') {
                    buffer_append(&buffer, ch);
                } 
                else if (ch == 'e' || ch == 'E') {
                    state = STATE_EXPONENT;
                    buffer_append(&buffer, ch);
                }
                else {
                    token_init(token, TOKEN_INTEGER, &buffer);
                    ungetc(ch, stream);
                    return token;
                }
                break;

            case STATE_DECIMAL_POINT:
                if (!(isdigit(ch) || ch == '0')) {
                    SCANNER_ERROR("Invalid decimal number")
                }
                else {
                    state = STATE_DECIMAL;
                    buffer_append(&buffer, ch);
                }
                break;

            case STATE_DECIMAL:
                if (isdigit(ch) || ch == '0') {
                    buffer_append(&buffer, ch);
                }
                else if (ch == 'e' || ch == 'E') {
                    state = STATE_EXPONENT;
                    buffer_append(&buffer, ch);
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
                    buffer_append(&buffer, ch);
                } 
                else if (isdigit(ch) || ch == '0') {
                    empty_exp = false;
                    buffer_append(&buffer, ch);
                }
                else {
                    if (empty_exp) {
                        SCANNER_ERROR("Invalid exponent")
                    }
                    token_init(token, TOKEN_DOUBLE, &buffer);
                    ungetc(ch, stream);
                    return token;
                }
                break;

            case STATE_ESCAPE_SEQUENCE:
                esc_sqv_cnt++;
                if (esc_sqv_cnt == 1 && ch == '{') {
                    break;
                } 
                else if (esc_sqv_cnt == 2 && isxdigit(ch)) {
                    number_buffer[0] = ch;
                    break;
                }
                else if (esc_sqv_cnt == 3 && isxdigit(ch)) {
                    number_buffer[1] = ch;
                    continue;
                }
                else if ((esc_sqv_cnt == 3 || esc_sqv_cnt == 4) && ch == '}') {
                    number_buffer[esc_sqv_cnt-2] = '\0';
                    buffer_apend_hex_num(&buffer, number_buffer);
                    if (multiline_mode) {
                        state = STATE_MULTILINE_STRING;
                    } else {
                        state = STATE_STRING;
                    }
                    break;
                }
                else {
                    SCANNER_ERROR("Invalid escape sequence")
                }
                break;
        }
    }
}

