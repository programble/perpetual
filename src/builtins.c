#include <talloc.h>

#include "builtins.h"

lisp_value *builtin_atom(int argc, lisp_value *args, context *ctx)
{
    if (argc != 1) {
        // TODO: Set error
        (void) ctx;
        return NULL;
    }

    lisp_value *eval = lisp_value_eval(LISP_CONS_CAR(args), ctx);
    if (!eval) return NULL;
    int atom = eval->type != LISP_TYPE_CONS || LISP_CONS_NIL(eval);
    talloc_free(eval);

    if (atom)
        return lisp_value_new_symbol(talloc_strdup(NULL, "t"));
    else
        return lisp_value_new_cons_nil();
}

lisp_value *builtin_car(int argc, lisp_value *args, context *ctx)
{
    if (argc != 1) {
        // TODO: Set error
        (void) ctx;
        return NULL;
    }

    lisp_value *eval = lisp_value_eval(LISP_CONS_CAR(args), ctx);
    if (!eval) return NULL;
    if (eval->type != LISP_TYPE_CONS) {
        // TODO: Set error
        talloc_free(eval);
        return NULL;
    }

    lisp_value *car;
    if (LISP_CONS_NIL(eval))
        car = lisp_value_new_cons_nil();
    else
        car = lisp_value_dup(LISP_CONS_CAR(eval));

    talloc_free(eval);
    return car;
}

lisp_value *builtin_cdr(int argc, lisp_value *args, context *ctx)
{
    if (argc != 1) {
        // TODO: Set error
        (void) ctx;
        return NULL;
    }

    lisp_value *eval = lisp_value_eval(LISP_CONS_CAR(args), ctx);
    if (!eval) return NULL;
    if (eval->type != LISP_TYPE_CONS) {
        // TODO: Set error
        talloc_free(eval);
        return NULL;
    }

    lisp_value *cdr;
    if (LISP_CONS_NIL(eval))
        cdr = lisp_value_new_cons_nil();
    else
        cdr = lisp_value_dup(LISP_CONS_CDR(eval));

    talloc_free(eval);
    return cdr;
}

lisp_value *builtin_quote(int argc, lisp_value *args, context *ctx)
{
    if (argc != 1) {
        // TODO: Set error
        (void) ctx;
        return NULL;
    }

    return lisp_value_dup(LISP_CONS_CAR(args));
}

void builtins_bind(scope *sco)
{
    lisp_value *t = lisp_value_new_symbol(talloc_strdup(NULL, "t"));
    lisp_value_set_meta(t, metadata_new(__FILE__, __LINE__, 0));
    scope_set(sco, "t", t);

    lisp_value *nil = lisp_value_new_cons_nil();
    lisp_value_set_meta(nil, metadata_new(__FILE__, __LINE__, 0));
    scope_set(sco, "nil", nil);

#define BIND(x) scope_set(sco, #x, lisp_value_new_builtin(builtin_ ##x, metadata_new(__FILE__, __LINE__, 0)))
    BIND(atom);
    BIND(car);
    BIND(cdr);
    BIND(quote);
#undef BIND
}
