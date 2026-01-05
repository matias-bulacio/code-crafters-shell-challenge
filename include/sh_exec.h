#ifndef H_SH_EXEC_H
#define H_SH_EXEC_H

#include "z-libs/zvec-registered.h"
#include "z-libs/zstr.h"

zstr find_exec(zstr_view exec_name, zstr_view path);
bool try_exec_from_env_path(zvec_ShArgs args, int *ret);

#endif // H_SH_EXEC_H
