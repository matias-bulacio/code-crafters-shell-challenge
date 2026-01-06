#ifndef SH_ENV_H
#define SH_ENV_H

#include "z-libs/zstr.h"
zstr zgetenv_v(zstr_view v);

zstr zgetenv_s(zstr *s);

#endif // SH_ENV_H
