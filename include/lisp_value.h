#pragma once
#ifndef __LISP_VALUE_H__
#define __LISP_VALUE_H__

#include <assert.h>

#include "metadata.h"
#include "context.h"

enum lisp_type {
    LISP_TYPE_CONS,
    LISP_TYPE_SYMBOL,
    LISP_TYPE_INT,
};

typedef struct lisp_value {
    enum lisp_type type;
    void *value;
    metadata *meta;
} lisp_value;

struct lisp_cons {
    lisp_value *car;
    lisp_value *cdr;
};

#define LISP_CONS(x)   (assert(x->type == LISP_TYPE_CONS),   (struct lisp_cons*)(x->value))
#define LISP_INT(x)    (assert(x->type == LISP_TYPE_INT),    (int*)(x->value))
#define LISP_SYMBOL(x) (assert(x->type == LISP_TYPE_SYMBOL), (char*)(x->value))

#define LISP_CONS_CAR(x) LISP_CONS(x)->car
#define LISP_CONS_CDR(x) LISP_CONS(x)->cdr
#define LISP_CONS_NIL(x) !LISP_CONS(x)

// src/lisp_value/new.c
lisp_value *lisp_value_new_cons(lisp_value *car, lisp_value *cdr);
lisp_value *lisp_value_new_cons_nil(void);
lisp_value *lisp_value_new_int(int *value);
lisp_value *lisp_value_new_symbol(char *value);

// src/lisp_value/meta.c
void lisp_value_set_meta(lisp_value *this, metadata *meta);

// src/lisp_value/sprint.c
char *lisp_value_sprint(lisp_value *this);

// src/lisp_value/dup.c
lisp_value *lisp_value_dup(lisp_value *this);

// src/lisp_value/eval.c
lisp_value *lisp_value_eval(lisp_value *this, context *ctx);

#endif /* __LISP_VALUE_H__ */
