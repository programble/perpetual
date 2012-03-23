#include <talloc.h>
#include <stdarg.h>

#include "exception.h"

exception *exception_new(metadata *origin, callstack *backtrace, enum exception_type type, const char *format, ...)
{
    exception *this = talloc(NULL, exception);
    this->type = type;
    this->callstack = talloc_steal(this, callstack_dup(backtrace));
    callstack_push(this->callstack, origin);

    va_list ap;
    va_start(ap, format);
    this->message = talloc_vasprintf(this, format, ap);
    va_end(ap);

    return this;
}
