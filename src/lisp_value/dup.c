#include <talloc.h>
#include <assert.h>

#include "lisp_value.h"

lisp_value *lisp_value_dup(lisp_value *this)
{
    switch (this->type) {
    case LISP_TYPE_INT: {
        int *i = talloc(NULL, int);
        *i = *LISP_INT(this);
        return lisp_value_new_int(i);
    case LISP_TYPE_SYMBOL:
        return lisp_value_new_symbol(talloc_strdup(NULL, LISP_SYMBOL(this)));
    case LISP_TYPE_CONS:
        if (LISP_CONS_NIL(this))
            return lisp_value_new_cons_nil();
        else
            return lisp_value_new_cons(lisp_value_dup(LISP_CONS_CAR(this)), lisp_value_dup(LISP_CONS_CDR(this)));
    case LISP_TYPE_BUILTIN:
        return lisp_value_new_builtin(LISP_BUILTIN(this));
    default:
        assert(0); return NULL;
    }
    }
}
