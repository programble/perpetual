#pragma once
#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "metadata.h"
#include "callstack.h"

enum exception_type {
    EXCEPTION_ERROR,
};

typedef struct exception {
    enum exception_type type;
    char *message;
    callstack *callstack;
} exception;

exception *exception_new(metadata *origin, callstack *backtrace, enum exception_type type, const char *format, ...);

#endif
