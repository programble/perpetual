#pragma once
#ifndef __PARSER_H__
#define __PARSER_H__

#include "lisp_value.h"
#include "metadata.h"

#define PARSER_ERROR(p) (p->error != PARSER_ENONE)

typedef struct parser {
    char *data;
    int len;
    int index;
    metadata *meta;
    enum {
        PARSER_ENONE,
        PARSER_EEOF,
        PARSER_EMISMATCH,
    } error;
    char *edata;
} parser;

parser *parser_new(char *file, char *data);

void parser_perror(parser *p);

lisp_value *parser_parse(parser *p);

#endif /* __PARSER_H__ */
