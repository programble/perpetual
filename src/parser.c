#include <talloc.h>
#include <string.h>
#include <stdio.h>

#include "parser.h"

#define WHITESPACE " \t\n\r"

parser *parser_new(char *file, char *data)
{
    parser *new = talloc(NULL, parser);
    new->data = talloc_strdup(new, data);
    new->len = strlen(new->data);
    new->index = 0;
    new->meta = talloc_steal(new, metadata_new(file, 1, 1));
    new->error = PARSER_ENONE;
    return new;
}

void parser_delete(parser *p)
{
    talloc_free(p);
}

void parser_perror(parser *p)
{
    printf("%s:%i:%i: error: ", p->meta->file, p->meta->line, p->meta->col);
    switch (p->error) {
        case PARSER_ENONE:
            printf("the operation completed successfully\n");
            break;
        case PARSER_EEOF:
            // TODO: More info (while parsing what) (store in p)
            printf("unexpected eof\n");
            break;
        case PARSER_EMISMATCH:
            printf("mismatched parenthesis\n");
            break;
    }
}

char parser_next(parser *p)
{
    if (p->index < p->len) {
        p->index++;
        char c = p->data[p->index];
        if (c == '\n') {
            p->meta->line++;
            p->meta->col = 1;
        } else {
            p->meta->col++;
        }
        return c;
    }
    return 0;
}

void parser_skip_whitespace(parser *p)
{
    if (p->index == p->len)
        return;
    if (strchr(WHITESPACE, p->data[p->index]))
        while (strchr(WHITESPACE, parser_next(p)));
}

char *parser_read_until(parser *p, char *end)
{
    char *start = &p->data[p->index];
    int len;
    for (len = 1; !strchr(end, parser_next(p)) && p->index < p->len; len++);
    return talloc_strndup(NULL, start, len);
}

lisp_value *parser_parse_symbol(parser *p)
{
    char *sym = parser_read_until(p, WHITESPACE ")");
    lisp_value *value = lisp_value_new_symbol(sym);
    lisp_value_set_meta(value, metadata_copy(p->meta));
    return value;
}

lisp_value *parser_parse_int(parser *p)
{
    // TODO: Implement
    return NULL;
}

lisp_value *parser_parse_cons(parser *p)
{
    // TODO: Implement
    return NULL;
}

lisp_value *parser_parse(parser *p)
{
    // Do nothing if an error occurred
    // This should never be called after an error
    if (p->error != PARSER_ENONE) return NULL;

    parser_skip_whitespace(p);

    // Nothing more to parse (NULL with no error)
    if (p->index == p->len) return NULL;

    char c = p->data[p->index];

    if (c == ')') { // A mismatched close-paren
        p->error = PARSER_EMISMATCH;
        return NULL;
    }

    if (c == ';') { // Comment to end of line
        while (parser_next(p) != '\n' && p->index < p->len);
        return parser_parse(p);
    }

    if (c == '(')
        return parser_parse_cons(p);
    if ((c >= '0' && c <= '9') || c == '-')
        return parser_parse_int(p);
    return parser_parse_symbol(p);
}
