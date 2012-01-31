#include <talloc.h>

#include "lisp_value.h"
#include "metadata.h"

void lisp_value_set_meta(lisp_value *value, metadata *meta)
{
    value->meta = talloc_steal(value, meta);
}
