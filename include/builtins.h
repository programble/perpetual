#pragma once
#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "lisp_value.h"
#include "scope.h"

void builtins_bind(scope *sco);

lisp_value *builtin_atom(lisp_value *args, context *ctx);
lisp_value *builtin_car(lisp_value *args, context *ctx);
lisp_value *builtin_cdr(lisp_value *args, context *ctx);
lisp_value *builtin_quote(lisp_value *args, context *ctx);

#endif /* __BUILTINS_H__ */
