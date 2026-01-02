#include "../include/args_type.h"
#include "../include/z-libs/zvec-registered.h"

zvec_ShArgs parse_into_args(zstr_view cmd) {
	zvec_ShArgs r = zvec_init(ShArgs);

	zstr_split_iter	it = zstr_split_init(cmd, " ");
	zstr_view out;
	while (zstr_split_next(&it, &out)) {
		out = zstr_view_trim(out);
		if(out.len == 0) continue;
		zvec_push(&r, (sh_arg)out);
	}

	return r;
}
