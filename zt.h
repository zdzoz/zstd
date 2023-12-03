#ifndef ZT_H
#define ZT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zda.h"

#include <stdio.h>

typedef int (*_zt_func_t)(void);
typedef zda(_zt_func_t) _zt_da;

#define Z_SUCCESS 0
#define Z_FAILURE 1

#define ZT_ASSERT(cond)                                                                           \
    do {                                                                                          \
        if (!(cond)) {                                                                            \
            fprintf(stderr, "[ASSERT] %s -- %s:%d -> %s\n", __func__, __FILE__, __LINE__, #cond); \
            return Z_FAILURE;                                                                     \
        }                                                                                         \
    } while (0)

#define ZT_MAIN()                                               \
    _zt_da _zt_tests = { 0, 0, 0 };                             \
    int main(void)                                              \
    {                                                           \
        uint32_t _zt_pass = 0;                                  \
                                                                \
        for (uint32_t i = 0; i < _zt_tests.len; i++) {          \
            int ret = _zt_tests.data[i]();                      \
            if (ret == Z_SUCCESS)                               \
                _zt_pass++;                                     \
        }                                                       \
        printf("\n[Tests %d/%d]\n\n", _zt_pass, _zt_tests.len); \
                                                                \
        int ret = Z_SUCCESS;                                    \
        if (_zt_pass != _zt_tests.len)                          \
            ret = Z_FAILURE;                                    \
        zda_free(&_zt_tests);                                   \
        return ret;                                             \
    }

#define ZT(name)                                                    \
    static int test_##name(void);                                   \
    static void _zt_push_##name(void) __attribute__((constructor)); \
    static void _zt_push_##name(void)                               \
    {                                                               \
        extern _zt_da _zt_tests;                                    \
        zda_push(&_zt_tests, test_##name);                          \
    }                                                               \
    static int test_##name(void)

#ifdef __cplusplus
}
#endif
#endif // ZT_H
