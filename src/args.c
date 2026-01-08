#include "../include/args_type.h"
#include "../include/z-libs/zvec-registered.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>
#include <wctype.h>

// This function allocates a bunch of zstr stored in the returned vector. Remember to free them!
zvec_ShArgs parse_into_args(zstr_view cmd) {
    zvec_ShArgs ret = zvec_init(ShArgs);

    { // For some reason, zstr_is_valid_utf8 takes in a zstr, so we need to
      // allocate it and deallocate it.
        zstr z = zstr_from_view(cmd);
        if (!zstr_is_valid_utf8(&z))
            return ret;
        zstr_free(&z);
    }

    const char *runes = cmd.data;

#define PUSH_AND_CLEAR_ARG(zv, zs)                                             \
    do {                                                                       \
        zvec_push(&(zv), zs);                                                  \
        zstr_clear(&(zs));                                                     \
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

        if (r == '\'') {
            quote = '\'';
            continue;
        }

        if (iswspace(r) && zstr_len(&arg) != 0) {
            PUSH_AND_CLEAR_ARG(ret, arg);
            continue;
        };
        zstr_fmt(&arg, "%lc", r);
    }

    if (zstr_len(&arg) != 0)
        PUSH_AND_CLEAR_ARG(ret, arg);

    zstr_free(&arg);
#undef PUSH_AND_CLEAR_ARG

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
