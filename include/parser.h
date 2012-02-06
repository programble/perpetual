#pragma once
#ifndef __parser_H__
#define __parser_H__

#include "lisp_value.h"
#include "metadata.h"

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
} parser;

parser *parser_new(char *file, char *data);
void parser_delete(parser *p);

void parser_perror(parser *p);

lisp_value *parser_parse(parser *p);

#endif /* __parser_H__ */
