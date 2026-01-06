#include "../include/builtin.h"
#include "../include/builtin-map.h"
#include "../include/macros.h"
#include "../include/sh_exec.h"
#include "../include/z-libs/zmaps-registered.h"
#include "../include/z-libs/zstr.h"
#include "../include/z-libs/zvec-registered.h"
#include "include/sh_env.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
zmap_ShBuiltinMap builtin_map;
int cmp_zstr(zstr_view a, zstr_view b) {
    if (zstr_view_eq_view(a, b) == true)
        return 0;
    return 1;
}

uint32_t hash_zstr(zstr_view a, uint32_t seed) {
    return zmap_default_hash(a.data, a.len, seed);
};

void builtin_map_init() {
    builtin_map = zmap_init(ShBuiltinMap, hash_zstr, cmp_zstr);
#define ADD_TO_MAP(func) zmap_put(&builtin_map, ZSV(#func), func##_cmd);
    BUILTIN_LIST(ADD_TO_MAP)
#undef ADD_TO_MAP
}

int type_cmd(zvec_ShArgs v, char **env) {
    zstr_view name = *zvec_at(&v, 1);

    // Is it a builtin?
    sh_builtin *k = zmap_get(&builtin_map, name);
    if (k != NULL) {
        printf("%.*s is a shell builtin\n", (int)name.len, name.data);
        return 0;
    }

    REACHED("Inside type");
    // Is it an executable file?
    scope {
        zstr_autofree path = zstr_init();
        char *pathenv = getenv("PATH");
        if (pathenv != NULL) {
            DLN("PATH=%s", pathenv);
            zstr_fmt(&path, "%s", pathenv);
        } else {
            REACHED("PATH is empty");
        }
        zstr exec_path = find_exec(name, zstr_view_from(pathenv));
        if (zstr_len(&exec_path) != 0) { // FOUND
            printf("%.*s is " ZSTR_FMT "\n", (int)name.len, name.data,
                   ZSTR_ARG(exec_path));
            return 0;
            ;
        }
        zstr_free(&exec_path);
    }

    printf("%.*s: not found\n", (int)name.len, name.data);
    return 1;
}

int exit_cmd(zvec_ShArgs, char **env) { exit(EXIT_SUCCESS); }

int echo_cmd(zvec_ShArgs v, char **env) {
    for (int i = 0; i < v.length; i++) {
        zstr_view arg = *zvec_at(&v, i);
        if (i == 0)
            continue;
        if (i == 1) {
            printf("%.*s", (int)arg.len, arg.data);
        } else {
            printf(" %.*s", (int)arg.len, arg.data);
        }
    }
    printf("\n");
    return 0;
}

int pwd_cmd(zvec_ShArgs, char **) {
    size_t cap = 1024;
    char *pwd_c = malloc(cap);
    if (pwd_c == NULL)
        return 1;
    while (getcwd(pwd_c, cap) == NULL) { // FAILED
        cap *= 1.5;
        pwd_c = realloc(pwd_c, cap);
        if (pwd_c == NULL)
            return 1;
    }
    printf("%s\n", pwd_c);
    free(pwd_c);
    return 0;
}

int cd_cmd(zvec_ShArgs args, char **) {
    zstr_view dir_v;
    if (args.length > 1)
        dir_v = args.data[1];
    else
        dir_v = ZSV("~");

    zstr_autofree dir_zstr = zstr_from_view(dir_v);
    if (zstr_view_starts_with(dir_v, "~")) {
        bool slash_after_tilde = dir_v.len != 1 && dir_v.data[1] == '/';

        if (dir_v.len != 1 && !slash_after_tilde)
            goto not_HOME_prefix;

        zstr_autofree home = zgetenv_v(ZSV("HOME"));
        zstr_clear(&dir_zstr);

        zstr_view rest = zstr_sub(dir_v, 1, dir_v.len - 1);

        zstr_fmt(&dir_zstr, ZSTR_FMT "%.*s", ZSTR_ARG(home), (int)rest.len, rest.data);
    }
not_HOME_prefix:
    if (0 != chdir(zstr_cstr(&dir_zstr))) {
        printf("cd: %.*s: No such file or directory\n", (int)dir_v.len,
               dir_v.data);
        return 1;
    }
    return 0;
}
