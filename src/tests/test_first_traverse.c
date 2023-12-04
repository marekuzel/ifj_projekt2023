#include "../scanner.h"
#include "../first_traverse.h"
#include "../errors.h"
#include "../parser.h"
#include <stdio.h>

int main () {

    Parser_t *parser = calloc(1,sizeof(Parser_t));

    Parser_init(parser);

    Error err = find_allFuncDef(parser);
    printf("\nerror code %d\n",err);
    return 0;
}



