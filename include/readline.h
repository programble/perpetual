#pragma once
#ifndef __READLINE_H__
#define __READLINE_H__

#ifdef HAVE_READLINE
#define READLINE_FREE(x) free(x)
#include <readline/readline.h>
#include <readline/history.h>
#else
#define READLINE_FREE(x) talloc_free(x)
char *readline(const char *prompt);
void add_history(char *line);
#endif

#endif /* __READLINE_H__ */
