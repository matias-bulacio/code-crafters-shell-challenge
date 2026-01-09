#include "../include/args_type.h"
#include "../include/z-libs/zvec-registered.h"
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>
#include <wctype.h>

char *control_chars = "\"\\";

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
    bool escaped = false;
    bool escaping_allowed = true;
    bool escape_only_control = false;
    zstr arg = zstr_init();
    while (r = zstr_next_rune(&runes), r != 0 && r != WEOF) {
        if (quote == 0 || escaping_allowed) {
            if (escaped) {
                if (!escape_only_control) {
					goto escape_char;
                }

                for (char *p = control_chars; *p != '\0'; p++) {
                    if (*p == r) {
						goto escape_char;
                    };
                }

                zstr_push(&arg, '\\');
				goto escape_char;
            }

            if (r == '\\') {
                escaped = true;
                continue;
            }
        }
        if (quote != 0) {
            if (r != quote) {
                zstr_fmt(&arg, "%lc", r);
                continue;
            }
            quote = 0;
            escaping_allowed = true;
            escape_only_control = false;
            continue;
        }

        if (r == '\'' || r == '"') {
            quote = r;
            escaping_allowed = (r == '"');
            escape_only_control = (r == '"');
            continue;
        }

        if (iswspace(r)) {
            if (zstr_len(&arg) != 0) {
                PUSH_AND_RESET_ARG(ret, arg);
                continue;
            }
            continue;
        };
        goto add_char;
    escape_char:
        escaped = false;
    add_char:
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
