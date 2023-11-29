#include "zt.h"

#include <string.h>

#define Z_IMPL
#include "zda.h"
#include "zsv.h"

ZT(zsv)
{
    const char* full_name = "Oskar Zdziarski";

    zsv first_name = zsv_cstr_delim(full_name, ' ');
    zsv last_name = zsv_cstr(full_name + first_name.len + 1);

    ZT_ASSERT(zsv_eq(first_name, zsv_cstr("Oskar")) == 0);
    ZT_ASSERT(zsv_eq(last_name, zsv_cstr("Zdziarski")) == 0);
    ZT_ASSERT(first_name.len == 5);
    ZT_ASSERT(last_name.len == 9);

    return Z_SUCCESS;
}

ZT(zda_int)
{
    typedef zda(int) int_da;

    int_da nums = { 0 };
    zda_push(&nums, 1);
    zda_push(&nums, 2);
    zda_push(&nums, 3);
    zda_push(&nums, 4);
    zda_push(&nums, 5);

    ZT_ASSERT(nums.len == 5);
    ZT_ASSERT(nums.data[0] == 1);
    ZT_ASSERT(nums.data[1] == 2);
    ZT_ASSERT(nums.data[2] == 3);
    ZT_ASSERT(nums.data[3] == 4);
    ZT_ASSERT(nums.data[4] == 5);

    zda_free(&nums);
    return Z_SUCCESS;
}

ZT(zda_pushv_int) {
    typedef zda(int) int_da;

    int_da nums = { 0 };
    zda_pushv(int, &nums, 1, 2, 3, 4, 5);

    ZT_ASSERT(nums.len == 5);
    ZT_ASSERT(nums.data[0] == 1);
    ZT_ASSERT(nums.data[1] == 2);
    ZT_ASSERT(nums.data[2] == 3);
    ZT_ASSERT(nums.data[3] == 4);
    ZT_ASSERT(nums.data[4] == 5);

    zda_free(&nums);
    return Z_SUCCESS;
}

ZT(zda_cstr)
{
    typedef zda(const char*) str_da;

    str_da strs = { 0 };
    zda_push(&strs, "a.c");
    zda_push(&strs, "b.c");
    zda_push(&strs, "c.c");
    zda_push(&strs, "d.c");
    zda_push(&strs, "e.c");

    ZT_ASSERT(strs.len == 5);
    ZT_ASSERT(strcmp(strs.data[0], "a.c") == 0);
    ZT_ASSERT(strcmp(strs.data[1], "b.c") == 0);
    ZT_ASSERT(strcmp(strs.data[2], "c.c") == 0);
    ZT_ASSERT(strcmp(strs.data[3], "d.c") == 0);
    ZT_ASSERT(strcmp(strs.data[4], "e.c") == 0);

    zda_free(&strs);
    return 0;
}

ZT(zda_pushv_cstr)
{
    typedef zda(const char*) str_da;

    str_da strs = { 0 };
    zda_pushv(char*, &strs, "a.c", "b.c", "c.c", "d.c", "e.c");

    ZT_ASSERT(strs.len == 5);
    ZT_ASSERT(strcmp(strs.data[0], "a.c") == 0);
    ZT_ASSERT(strcmp(strs.data[1], "b.c") == 0);
    ZT_ASSERT(strcmp(strs.data[2], "c.c") == 0);
    ZT_ASSERT(strcmp(strs.data[3], "d.c") == 0);
    ZT_ASSERT(strcmp(strs.data[4], "e.c") == 0);

    zda_free(&strs);
    return 0;
}

ZT_MAIN()
