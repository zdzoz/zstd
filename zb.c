#define ZB_IMPL
#define ZB_INCL_ZDA
#include "zb.h"

int main(void)
{
    // main app
    typedef zda(const char*) str_da;
    str_da srcs = { 0 };
    zda_push(&srcs, "main.c");

    str_da flags = { 0 };
    zda_push(&flags, "-Wall");
    zda_push(&flags, "-Wextra");
    zda_push(&flags, "-Werror");
    zda_push(&flags, "-std=c99");
    zda_push(&flags, "-pedantic");

    zbinfo b = zb_init();
    b.srcs_len = srcs.len;
    b.srcs = srcs.data;
    b.flags_len = flags.len;
    b.flags = flags.data;
    b.out = "zstd";

    printf("[Build Info]\n");
    zb_info(&b);
    zb_build(&b);

    // tests
    str_da test_srcs = { 0 };
    zda_push(&test_srcs, "test.c");

    zbinfo test = zb_init();
    test.srcs_len = test_srcs.len;
    test.srcs = test_srcs.data;
    test.flags_len = b.flags_len;
    test.flags = b.flags;
    test.debug = Z_FALSE;
    test.out = "test";
    test.log_level = ERROR;

    zb_build(&test);
    zb_run(&test);

    zb_run(&b);

    zda_free(&srcs);
    zda_free(&flags);
    zda_free(&test_srcs);
    return 0;
}
