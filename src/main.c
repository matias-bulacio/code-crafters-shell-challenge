#include "../include/z-libs/zstr.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

zstr get_input() {
    char buf[1024];
    fgets(buf, 1024, stdin);

    zstr s = zstr_from(buf);
    zstr_trim(&s);
    return s;
}

void not_found(zstr_view cmd) {
    size_t first_space = cmd.len;
    for (int i = 0; i < cmd.len; i++) {
        if (isspace((int)cmd.data[i])) {
            first_space = i;
            break;
        }
    }
    zstr_view name = cmd;
    name.len = first_space;
    fprintf(stdout, "%.*s: command not found\n", (int)name.len, name.data);
}

int run(zstr_view cmd) {
	if(zstr_view_eq(cmd, "exit")) {
		exit(EXIT_SUCCESS);
	}
	
	not_found(cmd);
	return 1;
}

void repl() {
    printf("$ ");
    zstr cmd = get_input();
    run(zstr_as_view(&cmd));
}

int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);

    while (true) {
        repl();
    }

    return 0;
}
