#ifndef ZMAPS_REGISTERED_H
#define ZMAPS_REGISTERED_H

#include "../builtin.h" // sh_builtin

// X(KeyType, ValueType, ShortName)
#define REGISTER_ZMAP_TYPES(X) \
	X(zstr_view, sh_builtin, ShBuiltinMap)

#include "zmap.h"

#endif // ZMAPS_REGISTERED_H
