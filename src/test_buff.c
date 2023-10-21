#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

 int main(void) {
    BufferT buf;
    buff_ret_t buff_ret_c;
    buff_ret_c = buffer_init(&buf);
    char *str = "ahoj pyco!";
    for (int i = 0; i < 2000000; i++) {
        buff_ret_c = buffer_append(&buf,str[i%10]);
        if (buff_ret_c != BUFF_APPEND_SUCCES) {
            printf("fuck, %d\n", i);
            break;
        }
    }

    char *out = buffer_export(&buf);
    free(out);
    buffer_detor(&buf);

 }