#include <stdlib.h>
#include <assert.h>

#include "lisp_value.h"

lisp_value *lisp_value_call(lisp_value *this, lisp_value *args, context *ctx)
{
    switch (this->type) {
    case LISP_TYPE_INT:
    case LISP_TYPE_SYMBOL:
    case LISP_TYPE_CONS:
        // TODO: Set exception (not callable)
        return NULL;
    case LISP_TYPE_BUILTIN: {
        int argc = lisp_cons_count(args);
        callstack_push(ctx->callstack, this->meta);
        lisp_value *value = LISP_BUILTIN(this)(argc, args, ctx);
        callstack_pop(ctx->callstack);
        return value;
    }
    default:
        assert(0); return NULL;
    }
}
