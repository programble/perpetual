#include <stdio.h>
#include <talloc.h>

#ifndef HAVE_READLINE

char *readline(const char *prompt)
{
    printf("%s", prompt);

    int len = 16;
    char *line = talloc_array(NULL, char, len);
    for (int i = 0;; i++) {
        int c = getchar();

        if (c == EOF && i == 0) {
            talloc_free(line);
            return NULL;
        }

        if (i == len)
            line = talloc_realloc(NULL, line, char, len += 16);

        if (c == EOF || c == '\n') {
            line[i] = 0;
            break;
        }

        line[i] = c;
    }
    return line;
}

void add_history(/* char *line */)
{
    return;
}

#endif /* HAVE_READLINE */
