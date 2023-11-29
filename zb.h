#ifndef H_ZB
#define H_ZB

#ifdef __cplusplus
extern "C" {
#endif

/////// DYNAMIC ARRAY
#include <stdint.h>

#define Z_TRUE 1
#define Z_FALSE 0

#ifndef Z_NO_ALLOC
#include <stdlib.h>
#define Z_ALLOC(size) malloc(size)
#define Z_REALLOC(ptr, size) realloc(ptr, size)
#define Z_FREE(ptr) free(ptr)
#endif

#ifdef ZB_INCLUDE_ZDA

#define zda(type)          \
    struct {               \
        type* data;        \
        uint32_t len;      \
        uint32_t capacity; \
    }

typedef zda(void*) zda_voidp;

#define ZDA_INIT_CAP 8
#define _z_zda_push(da, item)                                                             \
    do {                                                                              \
        if ((da)->len >= (da)->capacity) {                                            \
            (da)->capacity = (da)->capacity == 0 ? ZDA_INIT_CAP : (da)->capacity * 2; \
            (da)->data = (void**)Z_REALLOC((da)->data, (da)->capacity * sizeof(*(da)->data)); \
        }                                                                             \
        (da)->data[(da)->len++] = (item);                                             \
    } while (0)

#define zda_push(da, ...) \
    do {                   \
        _z_zda_pushv((zda_voidp*)da, __VA_ARGS__, NULL); \
    } while (0)

#define zda_free(da)        \
    do {                    \
        Z_FREE((da)->data); \
        (da)->data = NULL;  \
        (da)->len = 0;      \
        (da)->capacity = 0; \
    } while (0)

typedef zda(const char*) zsrcs;
typedef zda(const char*) zflags;
static void __unused _z_zda_pushv(zda_voidp* da, ...);

#endif // ZB_INCLUDE_ZDA

typedef enum zb_log_level {
    ERROR = 0,
    WARN,
    INFO
} zb_log_level;

typedef struct zbinfo {
    uint32_t srcs_len;
    const char** srcs;
    uint32_t flags_len;
    const char** flags;
    const char* cc;
    const char* out;
    const char* outdir;
    uint8_t debug;
    zb_log_level log_level;
} zbinfo;

static zbinfo zb_init();
static void zb_info(zbinfo* b);
static void zb_build(zbinfo* b);
static void zb_run(zbinfo* b);

#ifdef __cplusplus
}
#endif
#endif // H_ZB

#ifdef ZB_IMPL

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#ifdef ZB_INCLUDE_ZDA

// FIXME: doesnt work with non pointers
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

#endif // ZB_INCLUDE_ZDA

static zbinfo zb_init()
{
    zbinfo b = {
        .srcs_len = 0,
        .srcs = NULL,
        .flags_len = 0,
        .flags = NULL,
        .cc = "cc",
        .out = "a.out",
        .outdir = "build",
        .debug = Z_TRUE,
        .log_level = INFO,
    };
    return b;
}

static void zb_info(zbinfo* b)
{
    printf("cc: %s\n", b->cc);
    printf("out: %s\n", b->out);
    printf("outdir: %s\n", b->outdir);
    printf("srcs[%d]: ", b->srcs_len);
    for (uint32_t i = 0; i < b->srcs_len; i++) {
        printf("%s ", b->srcs[i]);
    }
    printf("\b\n");

    printf("flags: ");
    for (uint32_t i = 0; i < b->flags_len; i++) {
        printf("%s ", b->flags[i]);
    }
    printf("\b\n");
}

// TODO: make string builder
static void zb_build(zbinfo* b)
{
    char cmd[1024] = { 0 };
    sprintf(cmd, "%s -o %s/%s", b->cc, b->outdir, b->out);
    if (b->debug) {
        sprintf(cmd, "%s -g", cmd);
    }
    for (uint32_t i = 0; i < b->srcs_len; i++) {
        sprintf(cmd, "%s %s", cmd, b->srcs[i]);
    }

    for (uint32_t i = 0; i < b->flags_len; i++) {
        sprintf(cmd, "%s %s", cmd, b->flags[i]);
    }

    if (b->log_level >= INFO)
        printf("Running: %s\n", cmd);

    // make outdir
    char outdir[1024] = { 0 };
    sprintf(outdir, "mkdir -p %s", b->outdir);
    int ret = system(outdir);
    if (ret != 0) {
        fprintf(stderr, "\nFailed to create output directory\n");
        exit(ret);
    }

    // TODO: multithreading
    ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "\nCompilation failed\n");
        exit(ret);
    }

    if (b->log_level >= INFO) {
        printf("Compiled: %s/%s\n", b->outdir, b->out);
    }
}

static void zb_run(zbinfo* b)
{
    int pid = fork();
    if (pid == 0) {
        char cmd[1024] = { 0 };
        sprintf(cmd, "%s/%s", b->outdir, b->out);
        int ret = system(cmd);
        if (ret != 0) {
            if (b->log_level >= ERROR) {
                fprintf(stderr, "\n[ERROR] Failed to run %s\n", cmd);
            }
            exit(ret);
        }
        exit(0);
    } else {
        int status = 0;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                if (b->log_level >= ERROR) {
                    fprintf(stderr, "\n[ERROR] %s exited with status %d\n", b->out, exit_status);
                }
                exit(exit_status);
            }
        }
    }
}

#ifdef __cplusplus
}
#endif
#endif // ZB_IMPL
