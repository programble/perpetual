#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <talloc.h>
#include <stdbool.h>
#include <string.h>
#include "readline.h"

#include "builtins.h"
#include "context.h"
#include "lisp_value.h"
#include "namespace.h"
#include "parser.h"
#include "scope.h"

// TODO: No hard-coded version
#define VERSION_MAJOR "0"
#define VERSION_MINOR "1"
#define VERSION_PATCH "0"

#define SPRINT_PRINT(type, obj) \
    do { \
        char *sprint = type##_sprint(obj); \
        printf("%s", sprint); \
        talloc_free(sprint); \
    } while (0)
#define SPRINT_PRINTLN(type, obj) \
    do { \
        char *sprint = type##_sprint(obj); \
        printf("%s\n", sprint); \
        talloc_free(sprint); \
    } while (0)

void print_version()
{
    printf("Perpetual " VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH "\n");
}

void print_usage()
{
    // TODO: Implement
}

void parse_eval(parser *p, context *ctx, bool print)
{
    lisp_value *value;

    while ((value = parser_parse(p))) {
        callstack_push(ctx->callstack, value->meta);

        lisp_value *eval = lisp_value_eval(value, ctx);

        if (eval) {
            if (print)
                SPRINT_PRINTLN(lisp_value, eval);
            talloc_free(eval);
            callstack_pop(ctx->callstack);
        } else {
            printf("Some kind of error occurred, but I don't have exceptions yet!\n");
            SPRINT_PRINT(callstack, ctx->callstack);
            callstack_clear(ctx->callstack);
        }

        talloc_free(value);
    }

    if (PARSER_ERROR(p))
        parser_perror(p);
}

void main_repl(bool prompt, context *ctx)
{
    if (prompt)
        print_version();

    char *line;
    while ((line = readline(prompt ? ">> " : ""))) {
        parser *p = parser_new("<stdin>", line);
        parse_eval(p, ctx, true);
        talloc_free(p);

        if (*line)
            add_history(line);
        READLINE_FREE(line);
    }
}

void main_eval(char *eval, context *ctx)
{
    parser *p = parser_new("<eval>", eval);
    parse_eval(p, ctx, true);
    talloc_free(p);
}

void main_file(char *filename, context *ctx)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror(filename);
        // TODO: exit with EXIT_FAILURE
        return;
    }

    fseek(file, 0, SEEK_END);
    long flen = ftell(file);
    rewind(file);

    char *fdata = talloc_array(NULL, char, flen);
    fread(fdata, sizeof(char), flen, file);

    parser *p = parser_new(filename, fdata);
    parse_eval(p, ctx, false);
    talloc_free(p);
    talloc_free(fdata);
}

int main(int argc, char **argv)
{
    static struct option long_options[] = {
        {"eval",      required_argument, NULL, 'e'},
        {"no-prompt", no_argument,       NULL, 'N'},
        {"help",      no_argument,       NULL, 'h'},
        {"version",   no_argument,       NULL, 'V'},
        {0, 0, 0, 0}
    };
    static char short_options[] = "e:NhV";

    char *opt_eval = NULL, *opt_file = NULL;
    bool opt_prompt = true;

    char o;
    while ((o = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (o) {
        case 'e': opt_eval = optarg;  break;
        case 'N': opt_prompt = false; break;
        case 'h': print_usage();      return EXIT_SUCCESS;
        case 'V': print_version();    return EXIT_SUCCESS;
        case '?':                     return EXIT_FAILURE;
        }
    }
    if (optind < argc)
        opt_file = argv[optind];

    // Set up root namespace and evaluation context
    namespace *root = namespace_new("", NULL);
    namespace *core = namespace_new("core", root);
    context *ctx = context_new(core);

    builtins_bind(ctx->scope);

    if (opt_file)
        main_file(opt_file, ctx);
    else if (opt_eval)
        main_eval(opt_eval, ctx);
    else
        main_repl(opt_prompt, ctx);

    talloc_free(root);
    talloc_free(ctx);

    return EXIT_SUCCESS;
}
