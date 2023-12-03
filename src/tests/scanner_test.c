#include <stdio.h>
#include "../scanner.h"
#include "../utils.h"

int main() {
    TokenT *next_token = generate_token();
    while (1) {
        if (next_token == NULL) {
            next_token = generate_token();
            printf("Invalid input\n");
            continue;
        }
        if (next_token->type == TOKEN_EOF) {
            break;
        }
        print_token(next_token);
        next_token = generate_token();
    }
    return 0;

}