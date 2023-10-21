#define BUFFER_CAP_S 256

typedef struct buffer_t {
    char *bytes;
    int cap;
    int length;
} BufferT;


typedef enum {
    BUFF_INIT_FAIL,
    BUFF_INIT_SUCCES,
    BUFF_APPEND_SUCCES,
    BUFF_APPEND_FAIL
    } buff_ret_t;


/**
 * Initilaize ADT buffer
*/
buff_ret_t buffer_init(BufferT *buffer);

/**
 * Appends ch to the end of buffer
*/
buff_ret_t buffer_append(BufferT *buffer,char chr);

/**
 * Clears buffer 
*/
void buffer_clear(BufferT *buffer);


/**
 * Exports data in buffer and returns pointer to it, to be freed
*/
char *buffer_export(BufferT *buffer);

/**
 * Properly cleans up buffer fro memory
*/
void buffer_detor (BufferT *buffer);