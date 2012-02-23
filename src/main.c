#include <stdlib.h>
#include <stdio.h>
#include <talloc.h>
#include "readline.h"

#include "lisp_value.h"
#include "parser.h"
#include "context.h"
#include "namespace.h"
#include "scope.h"
#include "builtins.h"

int main(int argc, char **argv)
{
    // TODO: Banner
    printf("Perpetual\n");

    // Set up root namespace and evaluation context
    namespace *root = namespace_new("", NULL);
    namespace *core = namespace_new("core", root);
    context *ctx = context_new(core);

    // Some test bindings
    scope_set(ctx->scope, "nil", lisp_value_new_cons_nil());
    scope_set(ctx->scope, "t", lisp_value_new_symbol(talloc_strdup(NULL, "t")));
    scope_set(ctx->scope, "quote", lisp_value_new_builtin(builtin_quote));

    char *line;
    while ((line = readline(">> "))) {
        parser *p = parser_new("<stdin>", line);
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
        talloc_free(p);

        add_history(line);
        READLINE_FREE(line);
    }

    talloc_free(root);
    talloc_free(ctx);

    return 0;
}
