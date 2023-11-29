#ifndef H_ZDA
#define H_ZDA

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdint.h>

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
        type* data;   \
        uint32_t len; \
        uint32_t cap; \
    }

typedef zda(void*) zda_voidp;

#define _z_zda_push(da, item)                                                        \
    do {                                                                         \
        if ((da)->len >= (da)->cap) {                                            \
            (da)->cap = (da)->cap == 0 ? ZDA_INIT_CAP : (da)->cap * 2;           \
            (da)->data = (void**)Z_REALLOC((da)->data, (da)->cap * sizeof(*(da)->data)); \
        }                                                                        \
        (da)->data[(da)->len++] = (item);                                        \
    } while (0)

#define zda_push(da, ...) \
    do {                   \
        _z_zda_pushv((zda_voidp*)da, __VA_ARGS__, NULL); \
    } while (0)

#define zda_pop(da) ((da)->len > 0 ? (da)->data[--(da)->len] : NULL)

#define zda_free(da)        \
    do {                    \
        Z_FREE((da)->data); \
        (da)->data = NULL;  \
        (da)->len = 0;      \
        (da)->cap = 0;      \
    } while (0)

static void __unused _z_zda_pushv(zda_voidp* da, ...);

#ifdef __cplusplus
}
#endif
#endif // H_ZDA

#ifdef Z_IMPL

#ifdef __cplusplus
extern "C" {
#endif

// FIXME: does not work with non pointers
static void __unused _z_zda_pushv(zda_voidp* da, ...) {
    va_list args;
    va_start(args, da);
    void* item = va_arg(args, void*);
    while (item) {
        _z_zda_push(da, item);
        item = va_arg(args, void*);
    }
    va_end(args);
}

#ifdef __cplusplus
}
#endif
#endif // Z_IMPL
