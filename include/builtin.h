#ifndef SHELL_BUILTIN_H
#define SHELL_BUILTIN_H

#include "include/z-libs/zvec-registered.h"
#include "z-libs/zstr.h"

typedef int (*sh_builtin)(zvec_ShArgs);

#define BUILTIN_LIST(X) \
	X(echo) \
	X(exit) \
	X(type) \

#endif // SHELL_BUILTIN_H
