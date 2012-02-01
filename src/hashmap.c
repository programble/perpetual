#include <assert.h>
#include <talloc.h>
#include <string.h>

#include "hashmap.h"

int hashmap_hash(char *key)
{
    // Stole this hash function from boredomist/inari
    // boredomist stole it from somewhere else

    int len = strlen(key);
    int hash = 0;

    for (int i = 0; i < len; i++) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

hashmap *hashmap_new()
{
    hashmap *new = talloc(NULL, hashmap);
    new->root = NULL;
    return new;
}

void hashmap_delete(hashmap *map)
{
    talloc_free(map);
}

hashmap_node *hashmap_node_new(hashmap_node *parent,
                               char *key, int hash,
                               void *value,
                               hashmap_node *lesser, hashmap_node *greater)
{
    hashmap_node *new = talloc(parent, hashmap_node);
    new->parent = parent;
    new->key = talloc_strdup(new, key);
    new->hash = hash;
    new->value = value;
    new->lesser = lesser;
    new->greater = greater;
    return new;
}

void hashmap_node_set(hashmap_node *node, char *key, int hash, void *value)
{
    if (hash > node->hash && !node->greater)
        node->greater = hashmap_node_new(node, key, hash, value, NULL, NULL);
    else if (hash < node->hash && !node->lesser)
        node->lesser = hashmap_node_new(node, key, hash, value, NULL, NULL);
    else if (hash > node->hash)
        hashmap_node_set(node->greater, key, hash, value);
    else if (hash < node->hash)
        hashmap_node_set(node->lesser, key, hash, value);
    else if (hash == node->hash)
        node->value = value;
    else
        assert(0);
}

void hashmap_set(hashmap *map, char *key, void *value)
{
    int hash = hashmap_hash(key);

    if (map->root)
        hashmap_node_set(map->root, key, hash, value);
    else {
        hashmap_node *root = talloc(map, hashmap_node);
        root->key = key;
        root->hash = hash;
        root->value = value;
        root->lesser = NULL;
        root->greater = NULL;
        root->parent = NULL;
        map->root = root;
    }
}

void *hashmap_node_get(hashmap_node *node, int hash)
{
    if (!node)
        return NULL;
    if (hash == node->hash)
        return node->value;
    if (hash > node->hash)
        return hashmap_node_get(node->greater, hash);
    if (hash < node->hash)
        return hashmap_node_get(node->lesser, hash);
    assert(0);
}

void *hashmap_get(hashmap *map, char *key)
{
    return hashmap_node_get(map->root, hashmap_hash(key));
}

void *hashmap_node_del(hashmap *map, hashmap_node *node, int hash)
{
    void *value = NULL;

    if (!node)
        return value;

    if (hash > node->hash)
        return hashmap_node_del(map, node->greater, hash);
    else if (hash < node->hash)
        return hashmap_node_del(map, node->lesser, hash);
    else {
        value = node->value;

        if (node->greater && node->lesser) {
            hashmap_node *successor = node->greater;
            while (successor->lesser)
                successor = successor->lesser;
            node->key = successor->key;
            node->hash = successor->hash;
            node->value = successor->value;
            if (successor == successor->parent->lesser)
                successor->parent->lesser = talloc_steal(successor->parent, successor->greater);
            else if (successor == successor->parent->greater)
                successor->parent->greater = talloc_steal(successor->parent, successor->greater);
            else assert(0);
            if (successor->greater)
                successor->greater->parent = successor->parent;
            talloc_free(successor);
        } else if (node->greater) {
            if (!node->parent)
                map->root = talloc_steal(map, node->greater);
            else if (node == node->parent->lesser)
                node->parent->lesser = talloc_steal(node->parent, node->greater);
            else if (node == node->parent->greater)
                node->parent->greater = talloc_steal(node->parent, node->greater);
            else assert(0);
            node->greater->parent = node->parent;
            talloc_free(node);
        } else if (node->lesser) {
            if (!node->parent)
                map->root = talloc_steal(map, node->lesser);
            else if (node == node->parent->lesser)
                node->parent->lesser = talloc_steal(node->parent, node->lesser);
            else if (node == node->parent->greater)
                node->parent->greater = talloc_steal(node->parent, node->lesser);
            else assert(0);
            node->lesser->parent = node->parent;
            talloc_free(node);
        } else {
            if (node->parent == NULL)
                map->root = NULL;
            else if (node == node->parent->lesser)
                node->parent->lesser = NULL;
            else if (node == node->parent->greater)
                node->parent->greater = NULL;
            else assert(0);
            talloc_free(node);
        }
    }

    return value;
}

void *hashmap_del(hashmap *map, char *key)
{
    if (map->root)
        return hashmap_node_del(map, map->root, hashmap_hash(key));
    else
        return NULL;
}