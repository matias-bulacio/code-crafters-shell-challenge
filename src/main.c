#include "../include/z-libs/zstr.h"
#include "../include/z-libs/zstr.h"
#include "../include/builtin-map.h"
#include "../include/z-libs/zvec-registered.h"
#include "../include/args_func.h"
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

int run(zvec_ShArgs args) {
	zstr_view name = *zvec_at(&args, 0);
	sh_builtin *bi = zmap_get(&builtin_map, name);
	if(bi == NULL) {
		REACHED("Not found!");
		not_found(name);
		return 1;
	}
	return (*bi)(args);
}

void repl() {
    printf("$ ");
    zstr cmd = get_input();
	zvec_ShArgs args = parse_into_args(zstr_as_view(&cmd));
    run(args);
    zstr_free(&cmd);
}

int main(int argc, char *argv[]) {
	builtin_map_init();
    // Flush after every printf
    setbuf(stdout, NULL);

    while (true) {
        repl();
    }

    return 0;
}
