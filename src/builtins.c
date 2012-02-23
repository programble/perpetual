#include "builtins.h"

lisp_value *builtin_quote(lisp_value *args, context *ctx)
{
    // TODO: Exception on multiple args?
    return lisp_value_dup(LISP_CONS_CAR(args));
}
