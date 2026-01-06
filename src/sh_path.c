#include "../include/sh_path.h"
#include "../include/sh_env.h"

zstr sh_resolve_path(zstr_view path) {
	zstr dir_zstr = zstr_from_view(path);
    if (zstr_view_starts_with(path, "~")) {
        bool slash_after_tilde = path.len != 1 && path.data[1] == '/';

        if (path.len != 1 && !slash_after_tilde)
            goto not_HOME_prefix;

        zstr_autofree home = zgetenv_v(ZSV("HOME"));
        zstr_clear(&dir_zstr);

        zstr_view rest = zstr_sub(path, 1, path.len - 1);

        zstr_fmt(&dir_zstr, ZSTR_FMT "%.*s", ZSTR_ARG(home), (int)rest.len,
                 rest.data);
    }
not_HOME_prefix:
	return dir_zstr;
}
