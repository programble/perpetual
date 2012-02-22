#include <talloc.h>
#include <stdio.h>

#include "callstack.h"

callstack *callstack_new()
{
    callstack *this = talloc(NULL, callstack);
    this->head = NULL;
    return this;
}

void callstack_push(callstack *this, char *call, metadata *meta)
{
    callstack_node *node = talloc(this, callstack_node);
    node->call = call;
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

void callstack_print(callstack *this)
{
    for (callstack_node *node = this->head; node; node = node->next) {
        if (node->call)
            printf("  %s:%i:%i: in '%s'\n", node->meta->file, node->meta->line, node->meta->col, node->call);
        else
            printf("  %s:%i:%i\n", node->meta->file, node->meta->line, node->meta->col);
    }
}
