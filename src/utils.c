#include "utils.h"
#include <stdlib.h>
#include <string.h>
buff_ret_t buffer_init(BufferT *buffer) {
    buffer->length = 0;
    buffer->cap = BUFFER_CAP_S;

    buffer->bytes = (char *) calloc(buffer->cap,1);
    
    if (buffer->bytes == NULL) 
        return BUFF_INIT_FAIL;

    return BUFF_INIT_SUCCES;
}


buff_ret_t buffer_append(BufferT *buffer, char chr) {
    if (buffer->length >= buffer->cap) {
        buffer->bytes = realloc(buffer->bytes,buffer->cap * 2);
        if (buffer->bytes == NULL)
            return BUFF_APPEND_FAIL;
        buffer->cap <<= 1; // buffer->cap *= 2;
    }

    buffer->bytes[buffer->length++] = chr;
    return BUFF_APPEND_SUCCES;  

}

void buffer_clear(BufferT *buffer) {
    buffer->length = 0;
}

char *buffer_export(BufferT *buffer) {
    char * dst = (char * ) malloc(buffer->length);

    if (dst == NULL)
        return NULL;

    memcpy(dst,buffer->bytes,buffer->length);
    
    return dst;
}

void buffer_detor (BufferT *buffer) {
    free(buffer->bytes);
    buffer->bytes = NULL;
    buffer->cap = 0;
    buffer->length = 0;
}