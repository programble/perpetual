#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <talloc.h>

#include "lisp_value.h"
#include "scope.h"
#include "namespace.h"

lisp_value *lisp_value_eval_symbol(lisp_value *this, context *ctx)
{
    char *sym = LISP_SYMBOL(this);
    namespace *ns = ctx->ns;
    scope *sco = ctx->scope;

    while (strchr(sym, ':')) {
        // Find first namespace part
        int i = strcspn(sym, ".:");
        char *nsstr = talloc_strndup(NULL, sym, i);
        // Look up namespace
        namespace *newns = namespace_get(ns, nsstr);
        talloc_free(nsstr);

        if (newns) {
            sym += i + 1; // Consume the first namespace part
            ns = newns;
            sco = newns->scope;
        } else {
            // TODO: Set error
            return NULL;
        }
    }

    lisp_value *value = scope_get(sco, sym);
    if (value)
        return lisp_value_dup(value);
    else {
        // TODO: Set error
        return NULL;
    }
}

lisp_value *lisp_value_eval_cons(lisp_value *this, context *ctx)
{
    if (LISP_CONS_NIL(this))
        return lisp_value_dup(this);

    lisp_value *call = lisp_value_eval(LISP_CONS_CAR(this), ctx);
    if (!call)
        return NULL;
    lisp_value *value = lisp_value_call(call, LISP_CONS_CDR(this), ctx);
    talloc_free(call);
    return value;
}

lisp_value *lisp_value_eval(lisp_value *this, context *ctx)
{
    switch (this->type) {
    case LISP_TYPE_INT:
        return lisp_value_dup(this);
    case LISP_TYPE_SYMBOL:
        return lisp_value_eval_symbol(this, ctx);
    case LISP_TYPE_CONS:
        return lisp_value_eval_cons(this, ctx);
    case LISP_TYPE_BUILTIN:
        return lisp_value_dup(this);
    default:
        assert(0); return NULL;
    }
}
