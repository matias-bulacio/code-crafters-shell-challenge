#ifndef SHELL_BUILTIN_H
#define SHELL_BUILTIN_H

#include "z-libs/zvec-registered.h"
#include "z-libs/zstr.h"

typedef int (*sh_builtin)(zvec_ShArgs, char **env);

#define BUILTIN_LIST(X) \
	X(echo) \
	X(exit) \
	X(type) \

#define predeclare(f) int f##_cmd(zvec_ShArgs cmd, char **env);
BUILTIN_LIST(predeclare)
#undef predeclare

#endif // SHELL_BUILTIN_H
