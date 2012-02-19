#pragma once
#ifndef __SCOPE_H__
#define __SCOPE_H__

#include "hashmap.h"
#include "lisp_value.h"

struct namespace;

typedef struct scope {
    struct scope *parent;
    struct namespace *ns;
    hashmap *map;
} scope;

scope *scope_new(scope *parent);

void scope_set(scope *sco, char *key, lisp_value *value);
lisp_value *scope_get(scope *sco, char *key);
void scope_del(scope *sco, char *key);

#endif /* __SCOPE_H__ */
