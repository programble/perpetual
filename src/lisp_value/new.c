#include <talloc.h>

#include "lisp_value.h"

static lisp_value *lisp_value_new(enum lisp_type type, void *value)
{
    lisp_value *this = talloc(NULL, lisp_value);
    this->type = type;
    this->value = talloc_steal(this, value);
    this->meta = NULL;
    return this;
}

lisp_value *lisp_value_new_int(int *value)
{
    return lisp_value_new(LISP_TYPE_INT, value);
}

lisp_value *lisp_value_new_symbol(char *value)
{
    return lisp_value_new(LISP_TYPE_SYMBOL, value);
}

lisp_value *lisp_value_new_cons(lisp_value *car, lisp_value *cdr)
{
    lisp_value *this = lisp_value_new(LISP_TYPE_CONS, talloc(NULL, struct lisp_cons));
    LISP_CONS_CAR(this) = talloc_steal(LISP_CONS(this), car);
    LISP_CONS_CDR(this) = talloc_steal(LISP_CONS(this), cdr);
    return this;
}

lisp_value *lisp_value_new_cons_nil(void)
{
    return lisp_value_new(LISP_TYPE_CONS, NULL);
}

lisp_value *lisp_value_new_builtin(lisp_builtin_func value, metadata *meta)
{
    lisp_value *this = talloc(NULL, lisp_value);
    this->type = LISP_TYPE_BUILTIN;
    this->value = value;
    lisp_value_set_meta(this, meta);
    return this;
}
