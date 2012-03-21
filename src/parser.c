#include <talloc.h>
#include <string.h>
#include <stdio.h>

#include "parser.h"

#define WHITESPACE " \t\n\r"
#define DELIMETER WHITESPACE ")"

#define PARSER_C(this) this->data[this->index]
#define PARSER_EOF(this) (this->index == this->len)
#define PARSER_SET_ERROR(this, e, d) this->error = e; this->edata = d

parser *parser_new(char *file, char *data)
{
    parser *this = talloc(NULL, parser);
    this->data = talloc_strdup(this, data);
    this->len = strlen(this->data);
    this->index = 0;
    this->meta = talloc_steal(this, metadata_new(file, 1, 1));
    this->error = PARSER_ENONE;
    return this;
}

void parser_perror(parser *this)
{
    printf("%s:%i:%i: error: ", this->meta->file, this->meta->line, this->meta->col);
    switch (this->error) {
        case PARSER_ENONE:
            printf("the operation completed successfully\n");
            break;
        case PARSER_EEOF:
            printf("unexpected eof while parsing %s\n", this->edata);
            break;
        case PARSER_EMISMATCH:
            printf("mismatched parenthesis\n");
            break;
        case PARSER_EINVALID:
            printf("invalid %s literal\n", this->edata);
            break;
    }
}

char parser_next(parser *this)
{
    if (this->index < this->len) {
        this->index++;
        char c = PARSER_C(this);
        if (c == '\n') {
            this->meta->line++;
            this->meta->col = 1;
        } else {
            this->meta->col++;
        }
        return c;
    }
    return 0;
}

void parser_skip_whitespace(parser *this)
{
    if (strchr(WHITESPACE, PARSER_C(this)))
        while (strchr(WHITESPACE, parser_next(this)) && !PARSER_EOF(this));
}

char *parser_read_until(parser *this, char *end)
{
    char *start = &PARSER_C(this);
    int len;
    for (len = 1; !strchr(end, parser_next(this)) && this->index < this->len; len++);
    return talloc_strndup(NULL, start, len);
}

lisp_value *parser_parse_int(parser *this)
{
    char *numstr = parser_read_until(this, DELIMETER);

    if (!strcmp(numstr, "-")) { // Construct a symbol instead
        return lisp_value_new_symbol(numstr);
    }

    // A bit of a hack to detect trailing garbage
    int *num = talloc(NULL, int);
    int valid;
    char garbage;
    valid = sscanf(numstr, "%i%c", num, &garbage);
    if (valid != 1) {
        PARSER_SET_ERROR(this, PARSER_EINVALID, "integer");
        talloc_free(numstr);
        talloc_free(num);
        return NULL;
    }
    talloc_free(numstr);

    return lisp_value_new_int(num);
}

lisp_value *parser_parse_symbol(parser *this)
{
    char *sym = parser_read_until(this, DELIMETER);
    return lisp_value_new_symbol(sym);
}

lisp_value *parser_parse_cons_cdr(parser *this)
{
    metadata *meta = metadata_dup(this->meta);

    parser_skip_whitespace(this);
    if (PARSER_EOF(this)) {
        PARSER_SET_ERROR(this, PARSER_EEOF, "cons");
        return NULL;
    }

    // End of list
    if (PARSER_C(this) == ')') {
        parser_next(this);
        lisp_value *value = lisp_value_new_cons_nil();
        lisp_value_set_meta(value, meta);
        return value;
    }

    lisp_value *cadr = parser_parse(this);
    if (!cadr) return NULL;
    lisp_value *cddr = parser_parse_cons_cdr(this);
    if (!cddr) {
        talloc_free(cadr);
        return NULL;
    }

    lisp_value *value = lisp_value_new_cons(cadr, cddr);
    lisp_value_set_meta(value, meta);
    return value;
}

lisp_value *parser_parse_cons(parser *this)
{
    // Skip open-paren and any whitespace
    parser_next(this);
    parser_skip_whitespace(this);

    if (PARSER_EOF(this)) {
        PARSER_SET_ERROR(this, PARSER_EEOF, "cons");
        return NULL;
    }

    // Empty list (nil)
    if (PARSER_C(this) == ')') {
        parser_next(this);
        return lisp_value_new_cons_nil();
    }

    lisp_value *car = parser_parse(this);
    if (!car) return NULL;

    parser_skip_whitespace(this);
    if (PARSER_EOF(this)) {
        PARSER_SET_ERROR(this, PARSER_EEOF, "cons");
        talloc_free(car);
        return NULL;
    }

    // Improper list
    if (PARSER_C(this) == '.') {
        parser_next(this);

        lisp_value *cdr = parser_parse(this);
        if (!cdr) {
            PARSER_SET_ERROR(this, PARSER_EEOF, "cons");
            talloc_free(car);
            return NULL;
        }

        parser_skip_whitespace(this);
        if (PARSER_EOF(this) || PARSER_C(this) != ')') {
            // TODO: Separate error for no close-paren
            PARSER_SET_ERROR(this, PARSER_EEOF, "cons");
            talloc_free(car);
            talloc_free(cdr);
            return NULL;
        }
        parser_next(this);

        return lisp_value_new_cons(car, cdr);
    }

    // Proper list
    lisp_value *cdr = parser_parse_cons_cdr(this);
    if (!cdr) {
        talloc_free(car);
        return NULL;
    }

    return lisp_value_new_cons(car, cdr);
}

lisp_value *parser_parse(parser *this)
{
    // This should never be called after an error
    assert(this->error == PARSER_ENONE);

    parser_skip_whitespace(this);

    // Skip comments
    if (PARSER_C(this) == ';')
        while (parser_next(this) != '\n' && !PARSER_EOF(this));

    // Nothing more to parse (NULL with no error)
    if (PARSER_EOF(this)) return NULL;

    // Make a copy of the metadata for the next parsed value
    metadata *meta = metadata_dup(this->meta);
    lisp_value *value = NULL;

    char c = PARSER_C(this);

    if (c == ')') // A mismatched close-paren
        this->error = PARSER_EMISMATCH;
    else if (c == '(')
        value = parser_parse_cons(this);
    else if ((c >= '0' && c <= '9') || c == '-')
        value = parser_parse_int(this);
    else
        value = parser_parse_symbol(this);

    if (value)
        lisp_value_set_meta(value, meta);
    else
        talloc_free(meta);
    return value;
}
