#include "../include/sh_exec.h"
#include "../include/macros.h"
#include "../include/z-libs/zvec-registered.h"
#include "../include/args_type.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

zstr find_exec(zstr_view exec_name, zstr_view path) {
    zstr_split_iter it = zstr_split_init(path, ":");
    zstr_view path_dir;

    zstr s = zstr_init();
    while (zstr_split_next(&it, &path_dir)) {
        zstr_clear(&s);

        if (zstr_ends_with(&s, "/"))
            zstr_pop_char(&s);
        DLN(ZSTR_FMT, ZSTR_ARG(s));

#define SH_ZVIEW_ARG(s) (int)(s).len, (s).data
        zstr_fmt(&s, "%.*s/%.*s", SH_ZVIEW_ARG(path_dir),
                 SH_ZVIEW_ARG(exec_name)); // Path is now ready and stored in s
#undef SH_ZVIEW_ARG

        DLN(ZSTR_FMT, ZSTR_ARG(s));

        struct stat stat_data;
        int stat_res = stat(zstr_cstr(&s), &stat_data);
        if (stat_res != 0) {
            REACHED("Not found here!");
            continue;
        }

        if (0 != (stat_data.st_mode & S_IXUSR)) { // Has execute permisions
            REACHED("Has permisions!");
            return s;
        }

        REACHED("No permitions");

        continue;
    }
    zstr_clear(&s);
    return s;
}

bool try_exec_from_env_path(zvec_ShArgs args, int *ret) {
    zstr_autofree path = zstr_init();
    char *pathenv = getenv("PATH");
    if (pathenv != NULL) {
        DLN("PATH=%s", pathenv);
        zstr_fmt(&path, "%s", pathenv);
    } else {
        REACHED("PATH is empty");
    }

    zstr_autofree exec_path =
        find_exec(*zvec_at(&args, 0), zstr_as_view(&path));
    if (zstr_len(&exec_path) == 0) { // Error
        return false;
    }

    pid_t p = fork();
    if (p < 0)
        return false;
    if (p == 0) { // Child
        size_t arg_count = args.length + 1; // NULL PTR FOR TERMINATOR

        char **p_args = calloc(arg_count, sizeof(const char *));
        if (p_args == NULL)
            exit(EXIT_FAILURE);

        zstr *z_args = calloc(arg_count - 1, sizeof(zstr)); // We dont need a null terminator here;
        if (z_args == NULL)
            exit(EXIT_FAILURE);

        for (size_t i = 0; i < arg_count - 1; i++) {
            z_args[i] = zstr_from_view(args.data[i]);
			p_args[i] = zstr_cstr(&z_args[i]);
        }
        execv(zstr_cstr(&exec_path), p_args);

        // Failed exec:
        exit(EXIT_FAILURE);
        return false;
    }

    int wstatus;
    waitpid(p, &wstatus, 0);
    if (WIFEXITED(wstatus)) {
        if (ret != NULL) {
            *ret = WEXITSTATUS(wstatus);
        }
    }

    return true;
}
