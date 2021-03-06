#pragma once
#ifndef __CALLSTACK_H__
#define __CALLSTACK_H__

#include "metadata.h"

typedef struct callstack_node {
    metadata *meta;
    struct callstack_node *next;
} callstack_node;

typedef struct callstack {
    callstack_node *head;
} callstack;

callstack *callstack_new();
callstack *callstack_dup(callstack *this);

void callstack_push(callstack *this, metadata *meta);
void callstack_pop(callstack *this);

char *callstack_sprint(callstack *this);

#endif /* __CALLSTACK_H__ */
