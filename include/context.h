#pragma once
#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "callstack.h"

// Including the proper headers here would cause issues
struct scope;
struct namespace;
struct lisp_value;

typedef struct context {
    struct namespace *ns;
    struct scope *scope;
    callstack *callstack;
    struct lisp_value *exception;
} context;

context *context_new(struct namespace *ns);

#endif /* __CONTEXT_H__ */
