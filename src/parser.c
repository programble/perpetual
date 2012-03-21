#include <talloc.h>
#include <string.h>
#include <stdio.h>

#include "parser.h"

#define WHITESPACE " \t\n\r"
#define DELIMETER WHITESPACE ")"

#define PARSER_C(p) p->data[p->index]
#define PARSER_EOF(p) (p->index == p->len)
#define PARSER_SET_ERROR(p, e, d) p->error = e; p->edata = d

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

void parser_perror(parser *p)
{
    printf("%s:%i:%i: error: ", p->meta->file, p->meta->line, p->meta->col);
    switch (p->error) {
        case PARSER_ENONE:
            printf("the operation completed successfully\n");
            break;
        case PARSER_EEOF:
            printf("unexpected eof while parsing %s\n", p->edata);
            break;
        case PARSER_EMISMATCH:
            printf("mismatched parenthesis\n");
            break;
        case PARSER_EINVALID:
            printf("invalid %s literal\n", p->edata);
            break;
    }
}

char parser_next(parser *p)
{
    if (p->index < p->len) {
        p->index++;
        char c = PARSER_C(p);
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
    if (strchr(WHITESPACE, PARSER_C(p)))
        while (strchr(WHITESPACE, parser_next(p)) && !PARSER_EOF(p));
}

char *parser_read_until(parser *p, char *end)
{
    char *start = &PARSER_C(p);
    int len;
    for (len = 1; !strchr(end, parser_next(p)) && p->index < p->len; len++);
    return talloc_strndup(NULL, start, len);
}

lisp_value *parser_parse_int(parser *p)
{
    char *numstr = parser_read_until(p, DELIMETER);

    if (!strcmp(numstr, "-")) { // Construct a symbol instead
        return lisp_value_new_symbol(numstr);
    }

    // A bit of a hack to detect trailing garbage
    int *num = talloc(NULL, int);
    int valid;
    char garbage;
    valid = sscanf(numstr, "%i%c", num, &garbage);
    if (valid != 1) {
        PARSER_SET_ERROR(p, PARSER_EINVALID, "integer");
        talloc_free(numstr);
        talloc_free(num);
        return NULL;
    }
    talloc_free(numstr);

    return lisp_value_new_int(num);
}

lisp_value *parser_parse_symbol(parser *p)
{
    char *sym = parser_read_until(p, DELIMETER);
    return lisp_value_new_symbol(sym);
}

lisp_value *parser_parse_cons_cdr(parser *p)
{
    metadata *meta = metadata_dup(p->meta);

    parser_skip_whitespace(p);
    if (PARSER_EOF(p)) {
        PARSER_SET_ERROR(p, PARSER_EEOF, "cons");
        return NULL;
    }

    // End of list
    if (PARSER_C(p) == ')') {
        parser_next(p);
        lisp_value *value = lisp_value_new_cons_nil();
        lisp_value_set_meta(value, meta);
        return value;
    }

    lisp_value *cadr = parser_parse(p);
    if (!cadr) return NULL;
    lisp_value *cddr = parser_parse_cons_cdr(p);
    if (!cddr) {
        talloc_free(cadr);
        return NULL;
    }

    lisp_value *value = lisp_value_new_cons(cadr, cddr);
    lisp_value_set_meta(value, meta);
    return value;
}

lisp_value *parser_parse_cons(parser *p)
{
    // Skip open-paren and any whitespace
    parser_next(p);
    parser_skip_whitespace(p);

    if (PARSER_EOF(p)) {
        PARSER_SET_ERROR(p, PARSER_EEOF, "cons");
        return NULL;
    }

    // Empty list (nil)
    if (PARSER_C(p) == ')') {
        parser_next(p);
        return lisp_value_new_cons_nil();
    }

    lisp_value *car = parser_parse(p);
    if (!car) return NULL;

    parser_skip_whitespace(p);
    if (PARSER_EOF(p)) {
        PARSER_SET_ERROR(p, PARSER_EEOF, "cons");
        talloc_free(car);
        return NULL;
    }

    // Improper list
    if (PARSER_C(p) == '.') {
        parser_next(p);

        lisp_value *cdr = parser_parse(p);
        if (!cdr) {
            PARSER_SET_ERROR(p, PARSER_EEOF, "cons");
            talloc_free(car);
            return NULL;
        }

        parser_skip_whitespace(p);
        if (PARSER_EOF(p) || PARSER_C(p) != ')') {
            // TODO: Separate error for no close-paren
            PARSER_SET_ERROR(p, PARSER_EEOF, "cons");
            talloc_free(car);
            talloc_free(cdr);
            return NULL;
        }
        parser_next(p);

        return lisp_value_new_cons(car, cdr);
    }

    // Proper list
    lisp_value *cdr = parser_parse_cons_cdr(p);
    if (!cdr) {
        talloc_free(car);
        return NULL;
    }

    return lisp_value_new_cons(car, cdr);
}

lisp_value *parser_parse(parser *p)
{
    // This should never be called after an error
    assert(p->error == PARSER_ENONE);

    parser_skip_whitespace(p);

    // Skip comments
    if (PARSER_C(p) == ';')
        while (parser_next(p) != '\n' && !PARSER_EOF(p));

    // Nothing more to parse (NULL with no error)
    if (PARSER_EOF(p)) return NULL;

    // Make a copy of the metadata for the next parsed value
    metadata *meta = metadata_dup(p->meta);
    lisp_value *value = NULL;

    char c = PARSER_C(p);

    if (c == ')') // A mismatched close-paren
        p->error = PARSER_EMISMATCH;
    else if (c == '(')
        value = parser_parse_cons(p);
    else if ((c >= '0' && c <= '9') || c == '-')
        value = parser_parse_int(p);
    else
        value = parser_parse_symbol(p);

    if (value)
        lisp_value_set_meta(value, meta);
    else
        talloc_free(meta);
    return value;
}
