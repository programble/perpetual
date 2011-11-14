#include <stdio.h>
#include <talloc.h>

#include "lisp_value.h"

int main(/*int argc, char **argv*/)
{
    lisp_value *a, *b, *c, *d, *e;
    a = lisp_value_new_symbol(talloc_strdup(NULL, "the"));
    b = lisp_value_new_symbol(talloc_strdup(NULL, "quick"));
    c = lisp_value_new_symbol(talloc_strdup(NULL, "brown"));
    d = lisp_value_new_symbol(talloc_strdup(NULL, "fox"));
    e = lisp_value_new_symbol(talloc_strdup(NULL, "jumps"));

    lisp_value *improper = lisp_value_new_cons(a, b);
    lisp_value *proper = lisp_value_new_cons(c, lisp_value_new_cons(d, lisp_value_new_cons(e, lisp_value_new_cons_nil())));

    char *s;
    s = lisp_value_sprint(improper);
    printf("%s\n", s);
    talloc_free(s);
    s = lisp_value_sprint(proper);
    printf("%s\n", s);
    talloc_free(s);

    lisp_value_delete(improper);
    lisp_value_delete(proper);
    return 0;
}
