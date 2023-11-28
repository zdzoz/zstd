#include "zt.h"

#define Z_IMPL
#include "zda.h"
#include "zsv.h"

ZT(test_zsv) {
    const char* full_name = "Oskar Zdziarski";

    zsv first_name = zsv_cstr_delim(full_name, ' ');
    zsv last_name = zsv_cstr(full_name + first_name.len + 1);

    ZT_ASSERT(zsv_eq(first_name, zsv_cstr("Oskar")) == 0);
    ZT_ASSERT(zsv_eq(last_name, zsv_cstr("Zdziarski")) == 0);
    ZT_ASSERT(first_name.len == 5);
    ZT_ASSERT(last_name.len == 9);

    return Z_SUCCESS;
}

ZT(test_zda)
{
    typedef zda(int) intda;

    intda nums = { 0 };
    zda_add(&nums, 1);
    zda_add(&nums, 2);
    zda_add(&nums, 3);
    zda_add(&nums, 4);
    zda_add(&nums, 5);

    ZT_ASSERT(nums.len == 5);
    ZT_ASSERT(nums.data[0] == 1);
    ZT_ASSERT(nums.data[1] == 2);
    ZT_ASSERT(nums.data[2] == 3);
    ZT_ASSERT(nums.data[3] == 4);
    ZT_ASSERT(nums.data[4] == 5);

    zda_free(&nums);
    return Z_SUCCESS;
}

ZT_MAIN()
