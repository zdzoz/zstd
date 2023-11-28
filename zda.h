#ifndef H_ZDA
#define H_ZDA

#include <stdint.h>

#ifndef Z_NO_ALLOC
#include <stdlib.h>
#define Z_ALLOC(size) malloc(size)
#define Z_REALLOC(ptr, size) realloc(ptr, size)
#define Z_FREE(ptr) free(ptr)
#endif

#define ZDA_INIT_CAP 8

#define zda(type)     \
    struct {          \
        type* data;   \
        uint32_t len; \
        uint32_t cap; \
    }

#define zda_add(da, item)                                                        \
    do {                                                                         \
        if ((da)->len >= (da)->cap) {                                            \
            (da)->cap = (da)->cap == 0 ? ZDA_INIT_CAP : (da)->cap * 2;           \
            (da)->data = Z_REALLOC((da)->data, (da)->cap * sizeof(*(da)->data)); \
        }                                                                        \
        (da)->data[(da)->len++] = (item);                                        \
    } while (0)

#define zda_free(da)        \
    do {                    \
        Z_FREE((da)->data); \
        (da)->data = NULL;  \
        (da)->len = 0;      \
        (da)->cap = 0;      \
    } while (0)

#define zda_len(da) ((da)->len)

#endif // H_ZDA
