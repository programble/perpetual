#include <talloc.h>

#include "builtins.h"

lisp_value *builtin_atom(lisp_value *args, context *ctx)
{
    callstack_push(ctx->callstack, "atom", args->meta);

    if (LISP_CONS_NIL(args) || !LISP_CONS_NIL(LISP_CONS_CDR(args))) {
        // TODO: Set error
        (void) ctx;
        return NULL;
    }

    lisp_value *eval = lisp_value_eval(LISP_CONS_CAR(args), ctx);
    if (!eval) return NULL;
    int atom = eval->type != LISP_TYPE_CONS || LISP_CONS_NIL(eval);
    talloc_free(eval);
    callstack_pop(ctx->callstack);

    if (atom)
        return lisp_value_new_symbol(talloc_strdup(NULL, "t"));
    else
        return lisp_value_new_cons_nil();
}

lisp_value *builtin_quote(lisp_value *args, context *ctx)
{
    callstack_push(ctx->callstack, "quote", args->meta);

    if (LISP_CONS_NIL(args) || !LISP_CONS_NIL(LISP_CONS_CDR(args))) {
        // TODO: Set error
        (void) ctx;
        return NULL;
    }

    callstack_pop(ctx->callstack);

    return lisp_value_dup(LISP_CONS_CAR(args));
}

void builtins_bind(scope *sco)
{
    scope_set(sco, "t", lisp_value_new_symbol(talloc_strdup(NULL, "t")));
    scope_set(sco, "nil", lisp_value_new_cons_nil());

    scope_set(sco, "atom", lisp_value_new_builtin(builtin_atom));
    scope_set(sco, "quote", lisp_value_new_builtin(builtin_quote));
}
