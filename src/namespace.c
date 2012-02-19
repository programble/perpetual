#include <talloc.h>

#include "namespace.h"

namespace *namespace_new(char *name, namespace *parent)
{
    namespace *this = talloc(NULL, namespace);
    this->name = name;
    this->parent = parent;
    this->children = talloc_steal(this, hashmap_new());
    if (parent) {
        this->scope = scope_new(parent->scope);
        hashmap_set(parent->children, name, this);
    } else
        this->scope = scope_new(NULL);
    return this;
}
