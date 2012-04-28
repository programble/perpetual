#include <talloc.h>

#include "callstack.h"

callstack *callstack_new()
{
    callstack *this = talloc(NULL, callstack);
    this->head = NULL;
    return this;
}

callstack_node *callstack_node_dup(callstack_node *this)
{
    callstack_node *dup = talloc(NULL, callstack_node);
    dup->meta = talloc_steal(dup, metadata_dup(this->meta));
    dup->next = NULL;
    return dup;
}

callstack *callstack_dup(callstack *this)
{
    callstack *dup = talloc(NULL, callstack);
    dup->head = talloc_steal(dup, callstack_node_dup(this->head));
    callstack_node *dup_node = dup->head;
    for (callstack_node *node = this->head->next; node; node = node->next) {
        dup_node->next = talloc_steal(dup, callstack_node_dup(node));
        dup_node = dup_node->next;
    }
    return dup;
}

void callstack_push(callstack *this, metadata *meta)
{
    callstack_node *node = talloc(this, callstack_node);
    node->meta = talloc_steal(node, metadata_dup(meta));
    node->next = this->head;
    this->head = node;
}

void callstack_pop(callstack *this)
{
    callstack_node *node = this->head;
    this->head = node->next;
    talloc_free(node);
}

void callstack_clear(callstack *this)
{
    while (this->head)
        callstack_pop(this);
}

char *callstack_sprint(callstack *this)
{
    char *sprint = talloc_strdup(NULL, "");
    for (callstack_node *node = this->head; node; node = node->next) {
        if (node->meta->name)
            sprint = talloc_asprintf_append(sprint, "  %s:%i:%i %s\n", node->meta->file, node->meta->line, node->meta->col, node->meta->name);
        else
            sprint = talloc_asprintf_append(sprint, "  %s:%i:%i\n", node->meta->file, node->meta->line, node->meta->col);
    }
    return sprint;
}
