#include <talloc.h>

#include "namespace.h"

namespace *namespace_new(char *name, namespace *parent)
{
    namespace *new = talloc(NULL, namespace);
    new->name = name;
    new->parent = parent;
    new->children = talloc_steal(new, hashmap_new());
    if (parent) {
        new->bindings = scope_new(parent->bindings);
        hashmap_set(parent->children, name, new);
    } else
        new->bindings = scope_new(NULL);
    return new;
}
