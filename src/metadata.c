#include <talloc.h>

#include "metadata.h"

metadata *metadata_new(char *file, int line, int col)
{
    metadata *this = talloc(NULL, metadata);
    this->file = talloc_strdup(this, file);
    this->line = line;
    this->col = col;
    return this;
}

metadata *metadata_copy(metadata *this)
{
    return metadata_new(this->file, this->line, this->col);
}
