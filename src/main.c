#include "../include/args_func.h"
#include "../include/builtin-map.h"
#include "../include/macros.h"
#include "../include/z-libs/zstr.h"
#include "../include/z-libs/zvec-registered.h"
#include "../include/sh_exec.h"
#include <stddef.h>
#include <stdio.h>

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
    fprintf(stdout, "%.*s: command not found\n", (int)cmd.len, cmd.data);
}

int run(zvec_ShArgs args, char **env) {
    zstr_view name = zstr_as_view(zvec_at(&args, 0));
    sh_builtin *bi = zmap_get(&builtin_map, name);

	if (bi != NULL) return (*bi)(args, env);

	int ret;
	if(try_exec_from_env_path(args, &ret)) return ret;

	REACHED("Not found!");
	not_found(name);
	return 1;
}

void repl(char **env) {
    printf("$ ");
    zstr cmd = get_input();
    zvec_ShArgs args = parse_into_args(zstr_as_view(&cmd));
    run(args, env);
	zvec_foreach_decl(ShArgs, &args, it) {
		zstr_free(it);
	}
    zstr_free(&cmd);
}

int main(int argc, char *argv[], char *env[]) {
    builtin_map_init();
    // Flush after every printf
    setbuf(stdout, NULL);

    while (true) {
        repl(env);
    }

    return 0;
}
