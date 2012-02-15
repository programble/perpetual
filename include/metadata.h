#pragma once
#ifndef __METADATA_H__
#define __METADATA_H__

typedef struct metadata {
    char *file;
    int line;
    int col;
} metadata;

metadata *metadata_new(char *file, int line, int col);
metadata *metadata_copy(metadata *data);

#endif /* __METADATA_H__ */
