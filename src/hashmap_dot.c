#ifdef DEBUG

#include <stdio.h>

#include "hashmap_dot.h"

void hashmap_node_dot(hashmap_node *node)
{
    printf("%d [label=\"%s (%d) -> %s\"];\n", node->hash, node->key, node->hash, node->value);
    if (node->parent)
        printf("%d -> %d [label=\"parent\"];\n", node->hash, node->parent->hash);
    if (node->lesser) {
        printf("%d -> %d [label=\"lesser\"];\n", node->hash, node->lesser->hash);
        hashmap_node_dot(node->lesser);
    }
    if (node->greater) {
        printf("%d -> %d [label=\"greater\"];\n", node->hash, node->greater->hash);
        hashmap_node_dot(node->greater);
    }
}

void hashmap_dot(hashmap *map)
{
    printf("digraph G {\n");
    if (map->root)
        hashmap_node_dot(map->root);
    printf("}\n");
}

#endif /* DEBUG */
