#include <talloc.h>
#include <assert.h>

#include "lisp_value.h"

lisp_value *lisp_value_dup(lisp_value *this)
{
    lisp_value *copy;
    switch (this->type) {
    case LISP_TYPE_INT: {
        int *i = talloc(NULL, int);
        *i = *LISP_INT(this);
        copy = lisp_value_new_int(i);
        break;
    }
    case LISP_TYPE_SYMBOL:
        copy = lisp_value_new_symbol(talloc_strdup(NULL, LISP_SYMBOL(this)));
        break;
    case LISP_TYPE_CONS:
        if (LISP_CONS_NIL(this))
            copy = lisp_value_new_cons_nil();
        else
            copy = lisp_value_new_cons(lisp_value_dup(LISP_CONS_CAR(this)), lisp_value_dup(LISP_CONS_CDR(this)));
        break;
    case LISP_TYPE_BUILTIN:
        return lisp_value_new_builtin(LISP_BUILTIN(this), metadata_dup(this->meta));
    default:
        assert(0); return NULL;
    }
    lisp_value_set_meta(copy, metadata_dup(this->meta));
    return copy;
}
