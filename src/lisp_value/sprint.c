#include <talloc.h>

#include "lisp_value.h"

char *lisp_value_sprint_cons(lisp_value *this)
{
    char *car_s, *cdr_s = NULL, *s;
    car_s = lisp_value_sprint(LISP_CONS_CAR(this));
    if (LISP_CONS_CDR(this)->type == LISP_TYPE_CONS) {
        if (LISP_CONS_NIL(LISP_CONS_CDR(this))) {
            s = talloc_asprintf(NULL, "%s", car_s);
        } else {
            cdr_s = lisp_value_sprint_cons(LISP_CONS_CDR(this));
            s = talloc_asprintf(NULL, "%s %s", car_s, cdr_s);
        }
    } else {
        cdr_s = lisp_value_sprint(LISP_CONS_CDR(this));
        s = talloc_asprintf(NULL, "%s . %s", car_s, cdr_s);
    }
    talloc_free(car_s);
    talloc_free(cdr_s);
    return s;
}

char *lisp_value_sprint(lisp_value *this)
{
    switch (this->type) {
        case LISP_TYPE_CONS: {
            if (LISP_CONS_NIL(this))
                return talloc_strdup(NULL, "()");
            char *cons, *s = talloc_asprintf(NULL, "(%s)", cons = lisp_value_sprint_cons(this));
            talloc_free(cons);
            return s;
        }
        case LISP_TYPE_INT:
            return talloc_asprintf(NULL, "%i", *LISP_INT(this));
        case LISP_TYPE_SYMBOL:
            return talloc_asprintf(NULL, "%s", LISP_SYMBOL(this));
        default:
            assert(0); return NULL;
    }
}
