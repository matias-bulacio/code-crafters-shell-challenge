#include "../include/args_type.h"
#include "../include/z-libs/zvec-registered.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>
#include <wctype.h>

// This function allocates a bunch of zstr stored in the returned vector.
// Remember to free them!
zvec_ShArgs parse_into_args(const zstr cmd) {
    zvec_ShArgs ret = zvec_init(ShArgs);

    if (!zstr_is_valid_utf8(&cmd))
        return ret;

    const char *runes = zstr_cstr(&cmd);

#define PUSH_AND_RESET_ARG(zv, zs)                                             \
    do {                                                                       \
        zvec_push(&(zv), zs);                                                  \
        (zs) = zstr_init();                                                    \
    } while (0)

    wint_t r;
    wchar_t quote = 0;
    zstr arg = zstr_init();
    while (r = zstr_next_rune(&runes), r != 0 && r != WEOF) {
        if (quote != 0) {
            if (r != quote) {
                zstr_fmt(&arg, "%lc", r);
                continue;
            }
            quote = 0;
            continue;
        }

        if (r == '\'' || r == '"') {
            quote = r;
            continue;
        }

        if (iswspace(r)) {
            if (zstr_len(&arg) != 0) {
                PUSH_AND_RESET_ARG(ret, arg);
                continue;
            }
            continue;
        };
        zstr_fmt(&arg, "%lc", r);
    }

    if (zstr_len(&arg) != 0)
        PUSH_AND_RESET_ARG(ret, arg);

#undef PUSH_AND_RESET_ARG

    // zstr_split_iter it = zstr_split_init(cmd, " ");
    // zstr_view out;
    // while (zstr_split_next(&it, &out)) {
    //     out = zstr_view_trim(out);
    //     if (out.len == 0)
    //         continue;
    //     zvec_push(&ret, (sh_arg)out);
    // }

    return ret;
}
