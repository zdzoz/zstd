#ifndef H_ZB
#define H_ZB

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

#define zda(type)          \
    struct {               \
        type* data;        \
        uint32_t len;      \
        uint32_t capacity; \
    }

#define ZDA_INIT_CAP 8
#define zda_add(da, item)                                                             \
    do {                                                                              \
        if ((da)->len >= (da)->capacity) {                                            \
            (da)->capacity = (da)->capacity == 0 ? ZDA_INIT_CAP : (da)->capacity * 2; \
            (da)->data = Z_REALLOC((da)->data, (da)->capacity * sizeof(*(da)->data)); \
        }                                                                             \
        (da)->data[(da)->len++] = (item);                                             \
    } while (0)

#define zda_free(da)        \
    do {                    \
        Z_FREE((da)->data); \
        (da)->data = NULL;  \
        (da)->len = 0;      \
        (da)->capacity = 0; \
    } while (0)

#define zda_len(da) ((da)->len)

typedef zda(const char*) zsrcs;
typedef zda(const char*) zflags;

typedef enum zb_log_level {
    ERROR = 0,
    WARN,
    INFO
} zb_log_level;

typedef struct zbinfo {
    zsrcs srcs;
    zflags flags;
    const char* cc;
    const char* out;
    const char* outdir;
    uint8_t debug : 1;
    zb_log_level log_level;
} zbinfo;

#define zb_add(info, ...) _zb_add(info, __VA_ARGS__, NULL)
#define zb_flags(info, ...) _zb_flags(info, __VA_ARGS__, NULL)

static zbinfo zb_init();
static void zb_info(zbinfo* info);
static void zb_build(zbinfo* info);
static void zb_run(zbinfo* info);
static void zb_free(zbinfo* info);

static void _zb_add(zbinfo* info, ...);
static void _zb_flags(zbinfo* info, ...);

#endif // H_ZB

#ifdef ZB_IMPL

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

static void _zb_add(zbinfo* info, ...)
{
    va_list args;
    va_start(args, info);

    const char* src = va_arg(args, const char*);
    while (src != NULL) {
        zda_add(&(info)->srcs, src);
        src = va_arg(args, const char*);
    }

    va_end(args);
}

static void _zb_flags(zbinfo* info, ...)
{
    va_list args;
    va_start(args, info);

    const char* src = va_arg(args, const char*);
    while (src != NULL) {
        zda_add(&(info)->flags, src);
        src = va_arg(args, const char*);
    }

    va_end(args);
}

static zbinfo zb_init()
{
    zbinfo info = {
        .srcs = { 0 },
        .flags = { 0 },
        .cc = "cc",
        .out = "a.out",
        .outdir = "./build",
        .debug = Z_FALSE,
        .log_level = INFO,
    };
    return info;
}

static void zb_info(zbinfo* info)
{
    printf("cc: %s\n", info->cc);
    printf("out: %s\n", info->out);
    printf("outdir: %s\n", info->outdir);
    printf("srcs[%d]: ", zda_len(&info->srcs));
    for (uint32_t i = 0; i < zda_len(&info->srcs); i++) {
        printf("%s ", info->srcs.data[i]);
    }
    printf("\b\n");

    printf("flags: ");
    for (uint32_t i = 0; i < zda_len(&info->flags); i++) {
        printf("%s ", info->flags.data[i]);
    }
    printf("\b\n");
}

static void zb_free(zbinfo* info)
{
    zda_free(&info->srcs);
    zda_free(&info->flags);
}

// TODO: make string builder
static void zb_build(zbinfo* info)
{
    char cmd[1024] = { 0 };
    sprintf(cmd, "%s -o %s/%s", info->cc, info->outdir, info->out);
    if (info->debug) {
        sprintf(cmd, "%s -g", cmd);
    }
    for (uint32_t i = 0; i < zda_len(&info->srcs); i++) {
        sprintf(cmd, "%s %s", cmd, info->srcs.data[i]);
    }

    for (uint32_t i = 0; i < zda_len(&info->flags); i++) {
        sprintf(cmd, "%s %s", cmd, info->flags.data[i]);
    }

    if (info->log_level >= INFO)
        printf("Running: %s\n", cmd);

    // make outdir
    char outdir[1024] = { 0 };
    sprintf(outdir, "mkdir -p %s", info->outdir);
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

    if (info->log_level >= INFO) {
        printf("Compiled: %s/%s\n", info->outdir, info->out);
    }
}

static void zb_run(zbinfo* info)
{
    int pid = fork();
    if (pid == 0) {
        char cmd[1024] = { 0 };
        sprintf(cmd, "%s/%s", info->outdir, info->out);
        int ret = system(cmd);
        if (ret != 0) {
            if (info->log_level >= ERROR) {
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
                if (info->log_level >= ERROR) {
                    fprintf(stderr, "\n[ERROR] %s exited with status %d\n", info->out, exit_status);
                }
                exit(exit_status);
            }
        }
    }
}

#endif // ZB_IMPL
