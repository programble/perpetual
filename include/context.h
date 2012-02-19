#pragma once
#ifndef __CONTEXT_H__
#define __CONTEXT_H__

// Including scope.h and namespace.h here would cause issues
// They get included in context.c, so no worries
struct scope;
struct namespace;

// TODO: Add callstack, exception

typedef struct context {
    struct namespace *ns;
    struct scope *scope;
} context;

context *context_new(struct namespace *ns);

#endif /* __CONTEXT_H__ */
