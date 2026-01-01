#include "../include/z-libs/zstr.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEB_ENABLE
#define DLN(s, ...) fprintf(stderr, s "\n", __VA_ARGS__)
#define REACHED(s) DLN("Reached: %s:%s:%d, %s", __FILE__, __func__, __LINE__, s)
#else
#define DLN(...)
#define REACHED(s)
#endif

zstr get_input() {
    char buf[1024];
    fgets(buf, 1024, stdin);
    DLN("%lu", strlen(buf));

    REACHED("About to allocate!");
    zstr s = zstr_from(buf);
    REACHED("Allocated!");
    zstr_trim(&s);
    return s;
}

void not_found(zstr_view cmd) {
    size_t first_space = cmd.len;
    for (int i = 0; i < cmd.len; i++) {
        if (isspace((int)cmd.data[i])) {
            REACHED("Is space!");
            first_space = i;
            break;
        }
    }
    zstr_view name = cmd;
    name.len = first_space;
    fprintf(stdout, "%.*s: command not found\n", (int)name.len, name.data);
}

int run(zstr_view cmd) {
    if (zstr_view_eq(cmd, "exit")) {
        exit(EXIT_SUCCESS);
    }
    if (zstr_view_starts_with(cmd, "echo")) {

        zstr_split_iter it = zstr_split_init(cmd, " ");
        zstr_view arg;
        size_t argc = 0;
        zstr out = zstr_init();
        while (argc++, zstr_split_next(&it, &arg)) {
            if (argc == 1)
                continue;
            if (arg.len == 0)
                continue;
            if (argc == 2) {
                zstr_fmt(&out, "%.*s", (int)arg.len, arg.data);
            } else {
                zstr_fmt(&out, " %.*s", (int)arg.len, arg.data);
            }
        }
        printf(ZSTR_FMT "\n", ZSTR_ARG(out));
        zstr_free(&out);
        return 0;
    }
    REACHED("Will call not found");
    not_found(cmd);
    return 1;
}

void repl() {
    printf("$ ");
    zstr cmd = get_input();
    run(zstr_as_view(&cmd));
    zstr_free(&cmd);
}

int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);

    while (true) {
        repl();
    }

    return 0;
}
