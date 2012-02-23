#pragma once
#ifndef __CALLSTACK_H__
#define __CALLSTACK_H__

#include "metadata.h"

typedef struct callstack_node {
    char *call;
    metadata *meta;
    struct callstack_node *next;
} callstack_node;

typedef struct callstack {
    callstack_node *head;
} callstack;

callstack *callstack_new();

void callstack_push(callstack *this, char *call, metadata *meta);
void callstack_pop(callstack *this);
void callstack_clear(callstack *this);

void callstack_print(callstack *this);

#endif /* __CALLSTACK_H__ */
