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
    hashmap *this = talloc(NULL, hashmap);
    this->root = NULL;
    return this;
}

hashmap_node *hashmap_node_new(hashmap_node *parent,
                               char *key, int hash,
                               void *value,
                               hashmap_node *lesser, hashmap_node *greater)
{
    hashmap_node *this = talloc(parent, hashmap_node);
    this->parent = parent;
    this->key = talloc_strdup(this, key);
    this->hash = hash;
    this->value = talloc_steal(this, value);
    this->lesser = lesser;
    this->greater = greater;
    return this;
}

void hashmap_node_set(hashmap_node *this, char *key, int hash, void *value)
{
    if (hash > this->hash && !this->greater)
        this->greater = hashmap_node_new(this, key, hash, value, NULL, NULL);
    else if (hash < this->hash && !this->lesser)
        this->lesser = hashmap_node_new(this, key, hash, value, NULL, NULL);
    else if (hash > this->hash)
        hashmap_node_set(this->greater, key, hash, value);
    else if (hash < this->hash)
        hashmap_node_set(this->lesser, key, hash, value);
    else if (hash == this->hash) {
        talloc_free(this->value);
        this->value = talloc_steal(this, value);
    } else
        assert(0);
}

void hashmap_set(hashmap *this, char *key, void *value)
{
    int hash = hashmap_hash(key);

    if (this->root)
        hashmap_node_set(this->root, key, hash, value);
    else {
        hashmap_node *root = talloc(this, hashmap_node);
        root->key = key;
        root->hash = hash;
        root->value = talloc_steal(root, value);
        root->lesser = NULL;
        root->greater = NULL;
        root->parent = NULL;
        this->root = root;
    }
}

void *hashmap_node_get(hashmap_node *this, int hash)
{
    if (!this)
        return NULL;
    if (hash == this->hash)
        return this->value;
    if (hash > this->hash)
        return hashmap_node_get(this->greater, hash);
    if (hash < this->hash)
        return hashmap_node_get(this->lesser, hash);
    assert(0); return NULL;
}

void *hashmap_get(hashmap *this, char *key)
{
    return hashmap_node_get(this->root, hashmap_hash(key));
}

void hashmap_node_del(hashmap *map, hashmap_node *this, int hash)
{
    if (hash > this->hash && this->greater)
        return hashmap_node_del(map, this->greater, hash);
    else if (hash < this->hash && this->lesser)
        return hashmap_node_del(map, this->lesser, hash);
    else if (hash == this->hash) {
        talloc_free(this->value);

        if (this->greater && this->lesser) {
            hashmap_node *successor = this->greater;
            while (successor->lesser)
                successor = successor->lesser;
            this->key = successor->key;
            this->hash = successor->hash;
            this->value = successor->value;
            if (successor == successor->parent->lesser)
                successor->parent->lesser = talloc_steal(successor->parent, successor->greater);
            else if (successor == successor->parent->greater)
                successor->parent->greater = talloc_steal(successor->parent, successor->greater);
            else assert(0);
            if (successor->greater)
                successor->greater->parent = successor->parent;
            talloc_free(successor);
        } else if (this->greater) {
            if (!this->parent)
                map->root = talloc_steal(map, this->greater);
            else if (this == this->parent->lesser)
                this->parent->lesser = talloc_steal(this->parent, this->greater);
            else if (this == this->parent->greater)
                this->parent->greater = talloc_steal(this->parent, this->greater);
            else assert(0);
            this->greater->parent = this->parent;
            talloc_free(this);
        } else if (this->lesser) {
            if (!this->parent)
                map->root = talloc_steal(map, this->lesser);
            else if (this == this->parent->lesser)
                this->parent->lesser = talloc_steal(this->parent, this->lesser);
            else if (this == this->parent->greater)
                this->parent->greater = talloc_steal(this->parent, this->lesser);
            else assert(0);
            this->lesser->parent = this->parent;
            talloc_free(this);
        } else {
            if (this->parent == NULL)
                map->root = NULL;
            else if (this == this->parent->lesser)
                this->parent->lesser = NULL;
            else if (this == this->parent->greater)
                this->parent->greater = NULL;
            else assert(0);
            talloc_free(this);
        }
    }
}

void hashmap_del(hashmap *this, char *key)
{
    if (this->root)
        hashmap_node_del(this, this->root, hashmap_hash(key));
}

#ifdef DEBUG

#include <stdio.h>

void hashmap_node_dot(hashmap_node *this)
{
    printf("%d [label=\"%s (%d) -> %s\"];\n", this->hash, this->key, this->hash, (char *)this->value);
    if (this->parent)
        printf("%d -> %d [label=\"parent\"];\n", this->hash, this->parent->hash);
    if (this->lesser) {
        printf("%d -> %d [label=\"lesser\"];\n", this->hash, this->lesser->hash);
        hashmap_node_dot(this->lesser);
    }
    if (this->greater) {
        printf("%d -> %d [label=\"greater\"];\n", this->hash, this->greater->hash);
        hashmap_node_dot(this->greater);
    }
}

void hashmap_dot(hashmap *this)
{
    printf("digraph G {\n");
    if (this->root)
        hashmap_node_dot(this->root);
    printf("}\n");
}

#endif /* DEBUG */
