#include "../include/sh_env.h"
#include "../include/macros.h"

zstr zgetenv_s(zstr *s) {
    zstr var = zstr_init();
    char *var_c = getenv(zstr_cstr(s));
    if (var_c != NULL) {
        var = zstr_from(var_c);
    }

	return var;
}

zstr zgetenv_v(zstr_view v) {
	zstr_autofree s = zstr_from_view(v);
	return zgetenv_s(&s);
}
