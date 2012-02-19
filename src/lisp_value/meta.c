#include <talloc.h>

#include "lisp_value.h"
#include "metadata.h"

void lisp_value_set_meta(lisp_value *this, metadata *meta)
{
    this->meta = talloc_steal(this, meta);
}
