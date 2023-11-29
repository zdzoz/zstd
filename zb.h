#ifndef H_ZB
#define H_ZB

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define Z_TRUE 1
#define Z_FALSE 0

#ifdef ZB_INCL_ZDA

#include <stdarg.h>

#ifndef Z_NO_ALLOC
#include <stdlib.h>
#define Z_ALLOC(size) malloc(size)
#define Z_REALLOC(ptr, size) realloc(ptr, size)
#define Z_FREE(ptr) free(ptr)
#endif

#ifndef ZDA_INIT_CAP
#define ZDA_INIT_CAP 8
#endif

#define zda(type)     \
    struct {          \
        uint32_t len; \
        uint32_t cap; \
        type* data;   \
    }

#define zda_push(da, item)                                                       \
    do {                                                                         \
        if ((da)->len >= (da)->cap) {                                            \
            (da)->cap = (da)->cap == 0 ? ZDA_INIT_CAP : (da)->cap * 2;           \
            (da)->data = Z_REALLOC((da)->data, (da)->cap * sizeof(*(da)->data)); \
        }                                                                        \
        (da)->data[(da)->len++] = (item);                                        \
    } while (0)

#define zda_pop(da) ((da)->len > 0 ? (da)->data[--(da)->len] : NULL)

#define zda_free(da)        \
    do {                    \
        Z_FREE((da)->data); \
        (da)->data = NULL;  \
        (da)->len = 0;      \
        (da)->cap = 0;      \
    } while (0)

#define zda_pushv(type, da, ...)                       \
    do {                                               \
        type item[] = { __VA_ARGS__ };                 \
        size_t count = sizeof(item) / sizeof(item[0]); \
        for (size_t i = 0; i < count; i++)             \
            zda_push(da, item[i]);                     \
    } while (0)
#endif // ZB_INCL_ZDA

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
