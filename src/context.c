#include <talloc.h>

#include "context.h"
#include "scope.h"
#include "namespace.h"

context *context_new(namespace *ns)
{
    context *this = talloc(NULL, context);
    this->ns = ns;
    this->scope = ns->scope;
    return this;
}
