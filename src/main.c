#include <stddef.h>
#include <stdio.h>
#include "include/z-libs/zstr.h"

zstr get_input() {
	char buf[1024];
	fgets(buf, 1024, stdin);

	zstr s = zstr_from(buf);
	zstr_trim(&s);
	return s;
}

void not_found(zstr_view cmd) {
	zstr_split_iter it = zstr_split_init(cmd, " ");
	zstr_view name;
	zstr_split_next(&it, &name);

	fprintf(stdout, "%*s: command not found\n", (int)name.len, name.data);
}

int main(int argc, char *argv[]) {
    // Flush after every printf
    setbuf(stdout, NULL);

    // TODO: Uncomment the code below to pass the first stage
    printf("$ ");
	zstr s = get_input();
	not_found(zstr_as_view(&s));

    return 0;
}
