#define ZB_IMPL
#define ZB_INCL_ZDA
#include "../zb.h"

int main(void)
{
    typedef zda(const char*) str_da;
    str_da flags = { 0 };
    zda_push(&flags, "-Wall");
    zda_push(&flags, "-Wextra");
    zda_push(&flags, "-Werror");
    zda_push(&flags, "-std=c11");
    zda_push(&flags, "-pedantic");
    zda_push(&flags, "-I.");

    // tests
    str_da test_srcs = { 0 };
    zda_pushl(char*, &test_srcs, "tests/test.c", "tests/test_zda.c", "tests/test_zsv.c");

    zbinfo test = zb_init();
    test.srcs_len = test_srcs.len;
    test.srcs = test_srcs.data;
    test.flags_len = flags.len;
    test.flags = flags.data;
    test.debug = Z_FALSE;
    test.artifacts = (zb_artifacts){
        .out = "test",
        .bld_dir = "tests",
        .obj_dir = "tests/obj"
    };
    test.log_level = ERROR;

    printf("[Build Info]\n");
    zb_info(&test);
    zb_build(&test);
    zb_run(&test);

    zda_free(&flags);
    zda_free(&test_srcs);
    return 0;
}
