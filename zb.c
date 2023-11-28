#include <string.h>

#define ZB_IMPL
#include "zb.h"

int main(void)
{
    // main app
    zbinfo build = zb_init();
    zb_add(&build, "main.c");
    zb_flags(&build, "-Wall", "-Wextra", "-Werror", "-std=c99", "-pedantic"); // cflags
    zb_flags(&build, "-fsanitize=address,undefined"); // lflags
    build.debug = Z_TRUE;
    build.out = "zstd";

    printf("[Build Info]\n");
    zb_info(&build);
    zb_build(&build);

    // tests
    zbinfo test = zb_init();
    zb_add(&test, "test.c");
    test.flags = build.flags;
    test.debug = Z_FALSE;
    test.out = "test";
    test.log_level = ERROR;
    test.outdir = "build";

    zb_build(&test);
    zb_run(&test);

    zb_run(&build);

    zb_free(&build);
    // cleanup shared ptr
    test.flags.data = NULL;
    zb_free(&test);
    return 0;
}
