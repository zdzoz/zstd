#ifndef H_ZDA
#define H_ZDA

#ifdef __cplusplus
extern "C" {
#endif

#ifndef Z_NO_ALLOC
#include <stdlib.h>
#define Z_ALLOC(size) malloc(size)
#define Z_REALLOC(ptr, size) realloc(ptr, size)
#define Z_FREE(ptr) free(ptr)
#endif

#ifndef ZDA_INIT_CAP
#define ZDA_INIT_CAP 8
#endif

#define zda(type)     \
    struct {          \
        uint32_t len; \
        uint32_t cap; \
        type* data;   \
    }

#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#define zda_push(da, item)                                                       \
    do {                                                                         \
        if ((da)->len >= (da)->cap) {                                            \
            (da)->cap = (da)->cap == 0 ? ZDA_INIT_CAP : (da)->cap * 2;           \
            (da)->data = Z_REALLOC((da)->data, (da)->cap * sizeof(*(da)->data)); \
        }                                                                        \
        (da)->data[(da)->len++] = (item);                                        \
    } while (0)

#define zda_pushl(type, da, ...)                       \
    do {                                               \
        type item[] = { __VA_ARGS__ };                 \
        size_t count = sizeof(item) / sizeof(item[0]); \
        for (size_t i = 0; i < count; i++)             \
            zda_push(da, item[i]);                     \
    } while (0)

#define zda_pop(da) ((da)->len > 0 ? (da)->data[--(da)->len] : NULL)

#define zda_free(da)        \
    do {                    \
        Z_FREE((da)->data); \
        (da)->data = NULL;  \
        (da)->len = 0;      \
        (da)->cap = 0;      \
    } while (0)

#ifdef __cplusplus
}
#endif
#endif // H_ZDA
