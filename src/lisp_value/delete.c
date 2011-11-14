#include <talloc.h>

#include "lisp_value.h"

void lisp_value_delete(lisp_value *value)
{
    talloc_free(value);
}
