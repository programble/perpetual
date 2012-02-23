#pragma once
#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include "lisp_value.h"

lisp_value *builtin_quote(lisp_value *args, context *ctx);

#endif /* __BUILTINS_H__ */
