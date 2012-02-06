#include <stdlib.h>
#include <stdio.h>
#include <talloc.h>
#include "readline.h"

#include "lisp_value.h"
#include "parser.h"

int main(int argc, char **argv)
{
    // TODO: Banner
    printf("Perpetual\n");

    char *line;
    while ((line = readline(">> "))) {
        parser *p = parser_new("<stdin>", line);
        lisp_value *value;
        while ((value = parser_parse(p))) {
            char *sprint = lisp_value_sprint(value);
            printf("%s\n", sprint);
            talloc_free(sprint);
            lisp_value_delete(value);
        }
        // TODO: Macro for this test?
        if (PARSER_ERROR(p))
            parser_perror(p);
        parser_delete(p);
        add_history(line);
        READLINE_FREE(line);
    }

    return 0;
}
