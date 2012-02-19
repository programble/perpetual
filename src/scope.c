#include <talloc.h>

#include "scope.h"

scope *scope_new(scope *parent)
{
    scope *this = talloc(parent, scope);
    this->parent = parent;
    this->map = talloc_steal(this, hashmap_new());
    return this;
}

void scope_set(scope *this, char *key, lisp_value *value)
{
    hashmap_set(this->map, key, value);
}

lisp_value *scope_get(scope *this, char *key)
{
    lisp_value *value = hashmap_get(this->map, key);
    if (value)
        return value;
    else if (this->parent)
        return scope_get(this->parent, key);
    else
        return NULL;
}

void scope_del(scope *this, char *key)
{
    hashmap_del(this->map, key);
}
