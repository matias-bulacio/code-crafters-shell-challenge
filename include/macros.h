#ifndef H_CUSTOM_MACROS
#define H_CUSTOM_MACROS

#define scope if(0==0)
#ifdef DEB_ENABLE
#define DLN(s, ...) fprintf(stderr, s "\n", __VA_ARGS__)
#define REACHED(s) DLN("Reached: %s:%s:%d, %s", __FILE__, __func__, __LINE__, s)
#else
#define DLN(...)
#define REACHED(s)
#endif

#endif //H_CUSTOM_MACROS

