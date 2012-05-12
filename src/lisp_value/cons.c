#include "lisp_value.h"

int lisp_cons_count(lisp_value *this)
{
    int x = 0;
    for (lisp_value *i = this; !LISP_CONS_NIL(i); i = LISP_CONS_CDR(i), x++);
    return x;
}
