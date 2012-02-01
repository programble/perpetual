#pragma once
#ifndef __HASHMAP_H__
#define __HASHMAP_H__

typedef struct hashmap_node {
    char *key;
    int hash;
    void *value;
    struct hashmap_node *lesser, *greater, *parent;
} hashmap_node;

typedef struct hashmap {
    hashmap_node *root;
} hashmap;

int hashmap_hash(char *key);

hashmap *hashmap_new();
void hashmap_delete(hashmap *map);

void hashmap_set(hashmap *map, char *key, void *value);
void *hashmap_get(hashmap *map, char *key);
void *hashmap_del(hashmap *map, char *key);

#endif /* __HASHMAP_H__ */
