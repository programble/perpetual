#include <talloc.h>
#include <stdarg.h>

#include "exception.h"

exception *exception_new(callstack *backtrace, enum exception_type type, const char *format, ...)
{
    exception *this = talloc(NULL, exception);
    this->type = type;
    this->callstack = talloc_steal(this, callstack_dup(backtrace));

    va_list ap;
    va_start(ap, format);
    this->message = talloc_vasprintf(this, format, ap);
    va_end(ap);

    return this;
}

static const char *exception_string[] = {
    "Error",
    "ArgumentError",
};

char *exception_sprint(exception *this)
{
    char *sprint = talloc_asprintf(NULL, "%s: %s\n", exception_string[this->type], this->message);
    char *callstack = callstack_sprint(this->callstack);
    sprint = talloc_asprintf_append(sprint, "%s", callstack);
    talloc_free(callstack);
    return sprint;
}
