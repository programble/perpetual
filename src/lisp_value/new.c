#include <talloc.h>

#include "lisp_value.h"

static lisp_value *lisp_value_new(enum lisp_type type, void *value)
{
    lisp_value *new = talloc(NULL, lisp_value);
    new->type = type;
    new->value = talloc_steal(new, value);
    new->meta = NULL;
    return new;
}

lisp_value *lisp_value_new_cons(lisp_value *car, lisp_value *cdr)
{
    lisp_value *new = lisp_value_new(LISP_TYPE_CONS, talloc(NULL, struct lisp_cons));
    LISP_CONS_CAR(new) = talloc_steal(LISP_CONS(new), car);
    LISP_CONS_CDR(new) = talloc_steal(LISP_CONS(new), cdr);
    return new;
}

lisp_value *lisp_value_new_cons_nil(void)
{
    return lisp_value_new(LISP_TYPE_CONS, NULL);
}

lisp_value *lisp_value_new_int(int *value)
{
    return lisp_value_new(LISP_TYPE_INT, value);
}

lisp_value *lisp_value_new_symbol(char *value)
{
    return lisp_value_new(LISP_TYPE_SYMBOL, value);
}

