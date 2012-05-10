#pragma once
#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "callstack.h"
#include "exception.h"

// Including scope.h and namespace.h here would cause issues
// They get included in context.c, so no worries
struct scope;
struct namespace;

typedef struct context {
    struct namespace *ns;
    struct scope *scope;
    callstack *callstack;
    exception *exception;
} context;

context *context_new(struct namespace *ns);

#endif /* __CONTEXT_H__ */
