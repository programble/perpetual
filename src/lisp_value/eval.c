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
    // TODO: Call function and whatever
    (void) ctx;
    return lisp_value_dup(this);
}

lisp_value *lisp_value_eval(lisp_value *this, context *ctx)
{
    callstack_push(ctx->callstack, NULL, this->meta);

    lisp_value *eval;
    switch (this->type) {
    case LISP_TYPE_INT:
        eval = lisp_value_dup(this);
        break;
    case LISP_TYPE_SYMBOL:
        eval = lisp_value_eval_symbol(this, ctx);
        break;
    case LISP_TYPE_CONS:
        eval = lisp_value_eval_cons(this, ctx);
        break;
    case LISP_TYPE_BUILTIN:
        eval = lisp_value_dup(this);
        break;
    default:
        assert(0); return NULL;
    }

    if (!eval)
        return NULL;
    callstack_pop(ctx->callstack);
    return eval;
}
