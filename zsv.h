#ifndef H_ZSV
#define H_ZSV

#include <stdio.h>

// TODO: make stbi like header library

static size_t _z_strlen(const char* str)
{
    size_t size = 0;
    while (*str++ != 0)
        size++;
    return size;
}

typedef struct zsv {
    const char* cstr;
    size_t len;
} zsv;

static zsv zsv_cstr(const char* str);
static zsv zsv_cstr_delim(const char* str, char delim);
static int zsv_eq(zsv a, zsv b);

#define zsv_fmt "%.*s"
#define zsv_arg(zsv) (int)(zsv.len), zsv.cstr

#endif // H_ZSV

#ifdef Z_IMPL

static zsv zsv_cstr(const char* str)
{
    zsv zsv = { .cstr = str, .len = _z_strlen(str) };
    return zsv;
}

static zsv zsv_cstr_delim(const char* str, char delim)
{
    size_t len = 0;
    const char* s = str;
    while (*s != delim && *s != 0) {
        len++;
        s++;
    }
    zsv zsv = { .cstr = str, .len = len };
    return zsv;
}

static int zsv_eq(zsv a, zsv b)
{
    if (a.len < b.len)
        return -1;
    if (a.len > b.len)
        return 1;

    size_t len = a.len;
    const char* s1 = a.cstr;
    const char* s2 = b.cstr;
    while (len && (*s1 == *s2)) {
        s1++;
        s2++;
        len--;
    }
    return len ? *s1 - *s2 : 0;
}

#endif // Z_IMPL
