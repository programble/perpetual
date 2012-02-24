#include <stdlib.h>
#include <stdio.h>
#include <talloc.h>
#include <getopt.h>
#include "readline.h"

#include "lisp_value.h"
#include "parser.h"
#include "context.h"
#include "namespace.h"
#include "scope.h"
#include "builtins.h"

#define VERSION_MAJOR "0"
#define VERSION_MINOR "1"
#define VERSION_PATCH "0"

void print_version()
{
    printf("Perpetual " VERSION_MAJOR "." VERSION_MINOR "." VERSION_PATCH "\n");
}

void print_usage()
{
    // TODO: Implement
}

void parse_eval(parser *p, context *ctx)
{
    lisp_value *value;

    while ((value = parser_parse(p))) {
        lisp_value *eval = lisp_value_eval(value, ctx);
        if (eval) {
            char *sprint = lisp_value_sprint(eval);
            printf("%s\n", sprint);
            talloc_free(sprint);
            talloc_free(eval);
        } else {
            printf("Some kind of error occurred, but I don't have exceptions yet!\n");
            callstack_print(ctx->callstack);
            callstack_clear(ctx->callstack);
        }
        talloc_free(value);
    }

    if (PARSER_ERROR(p))
        parser_perror(p);
}

void main_repl(context *ctx)
{
    print_version();

    char *line;
    while ((line = readline(">> "))) {
        parser *p = parser_new("<stdin>", line);
        parse_eval(p, ctx);
        talloc_free(p);

        add_history(line);
        READLINE_FREE(line);
    }
}

void main_eval(char *eval, context *ctx)
{
    parser *p = parser_new("<eval>", eval);
    parse_eval(p, ctx);
    talloc_free(p);
}

void main_file()
{
    // TODO: Implement
}

int main(int argc, char **argv)
{
    static struct option long_options[] = {
        {"eval",    required_argument, NULL, 'e'},
        {"help",    no_argument,       NULL, 'h'},
        {"version", no_argument,       NULL, 'V'},
        {0, 0, 0, 0}
    };
    static char short_options[] = "e:hV";

    char *opt_eval = NULL;

    char o;
    while ((o = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (o) {
        case 'e': opt_eval = optarg; break;
        case 'h': print_usage();     return EXIT_SUCCESS;
        case 'V': print_version();   return EXIT_SUCCESS;
        case '?':                    return EXIT_FAILURE;
        }
    }

    if (optind < argc) {
        // TODO: Filename is in argv[optind]
    }

    // Set up root namespace and evaluation context
    namespace *root = namespace_new("", NULL);
    namespace *core = namespace_new("core", root);
    context *ctx = context_new(core);

    builtins_bind(ctx->scope);

    if (opt_eval)
        main_eval(opt_eval, ctx);
    else
        main_repl(ctx);

    talloc_free(root);
    talloc_free(ctx);

    return 0;
}
