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
        this->scope = talloc_steal(this, scope_new(NULL));
    return this;
}

namespace *namespace_get(namespace *this, char *name)
{
    if (this->name == name)
        return this;
    namespace *ns = hashmap_get(this->children, name);
    if (ns)
        return ns;
    else if (this->parent)
        return namespace_get(this->parent, name);
    else
        return NULL;
}
