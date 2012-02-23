#include <stdlib.h>

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
        callstack_push(ctx->callstack, this->meta);
        lisp_value *value = LISP_BUILTIN(this)(args, ctx);
        if (value) {
            callstack_pop(ctx->callstack);
            return value;
        } else return NULL;
    }
    }
}
