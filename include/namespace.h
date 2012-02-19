#pragma once
#ifndef __NAMESPACE_H__
#define __NAMESPACE_H__

#include "hashmap.h"
#include "scope.h"

typedef struct namespace {
    char *name;
    struct namespace *parent;
    hashmap *children;
    scope *scope;
} namespace;

namespace *namespace_new(char *name, namespace *parent);

namespace *namespace_get(namespace *this, char *name);

#endif /* __NAMESPACE_H__ */
