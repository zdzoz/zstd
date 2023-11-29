#include <zt.h>

#define Z_IMPL
#include <zsv.h>

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
