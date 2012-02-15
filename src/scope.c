#include <talloc.h>

#include "scope.h"

scope *scope_new(scope *parent)
{
    scope *new = talloc(parent, scope);
    new->parent = parent;
    new->map = talloc_steal(new, hashmap_new());
    return new;
}

void scope_set(scope *sco, char *key, lisp_value *value)
{
    hashmap_set(sco->map, key, value);
}

lisp_value *scope_get(scope *sco, char *key)
{
    lisp_value *value = hashmap_get(sco->map, key);
    if (value)
        return value;
    else if (sco->parent)
        return scope_get(sco->parent, key);
    else
        return NULL;
}

void scope_del(scope *sco, char *key)
{
    hashmap_del(sco->map, key);
}
