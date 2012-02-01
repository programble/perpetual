#include <talloc.h>

#include "metadata.h"

metadata *metadata_new(char *file, int line, int col)
{
    metadata *new = talloc(NULL, metadata);
    new->file = talloc_strdup(new, file);
    new->line = line;
    new->col = col;
    return new;
}

void metadata_delete(metadata *data)
{
    talloc_free(data);
}