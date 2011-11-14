#include <talloc.h>

#include "lisp_value.h"

char *lisp_value_sprint_cons(lisp_value *value)
{
    char *car_s, *cdr_s, *s;
    car_s = lisp_value_sprint(LISP_CONS_CAR(value));
    if (LISP_CONS_CDR(value)->type == LISP_TYPE_CONS) {
        if (LISP_CONS_NIL(LISP_CONS_CDR(value))) {
            s = talloc_asprintf(NULL, "%s", car_s);
        } else {
            cdr_s = lisp_value_sprint_cons(LISP_CONS_CDR(value));
            s = talloc_asprintf(NULL, "%s %s", car_s, cdr_s);
        }
    } else {
        cdr_s = lisp_value_sprint(LISP_CONS_CDR(value));
        s = talloc_asprintf(NULL, "%s . %s", car_s, cdr_s);
    }
    talloc_free(car_s);
    talloc_free(cdr_s);
    return s;
}

char *lisp_value_sprint(lisp_value *value)
{
    switch (value->type) {
        case LISP_TYPE_CONS: {
            if (LISP_CONS_NIL(value))
                return talloc_strdup(NULL, "()");
            char *cons, *s = talloc_asprintf(NULL, "(%s)", cons = lisp_value_sprint_cons(value));
            talloc_free(cons);
            return s;
        }
        case LISP_TYPE_INT:
            return talloc_asprintf(NULL, "%i", *LISP_INT(value));
        case LISP_TYPE_SYMBOL:
            return talloc_asprintf(NULL, "%s", LISP_SYMBOL(value));
        default:
            assert(0);
    }
}
