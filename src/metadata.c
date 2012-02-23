#include <talloc.h>

#include "metadata.h"

metadata *metadata_new(char *file, int line, int col)
{
    metadata *this = talloc(NULL, metadata);
    this->file = talloc_strdup(this, file);
    this->line = line;
    this->col = col;
    this->name = NULL;
    return this;
}

metadata *metadata_dup(metadata *this)
{
    metadata *copy = metadata_new(this->file, this->line, this->col);
    copy->name = talloc_strdup(copy, this->name);
    return copy;
}
