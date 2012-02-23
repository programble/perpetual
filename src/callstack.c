#include <talloc.h>
#include <stdio.h>

#include "callstack.h"

callstack *callstack_new()
{
    callstack *this = talloc(NULL, callstack);
    this->head = NULL;
    return this;
}

void callstack_push(callstack *this, metadata *meta)
{
    callstack_node *node = talloc(this, callstack_node);
    node->meta = meta;
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

void callstack_print(callstack *this)
{
    for (callstack_node *node = this->head; node; node = node->next) {
        if (node->meta->name)
            printf("  %s:%i:%i: '%s'\n", node->meta->file, node->meta->line, node->meta->col, node->meta->name);
        else
            printf("  %s:%i:%i\n", node->meta->file, node->meta->line, node->meta->col);
    }
}
