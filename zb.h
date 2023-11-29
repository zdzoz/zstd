#ifndef H_ZB
#define H_ZB

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define Z_TRUE 1
#define Z_FALSE 0

#define ZB_INCL_ZDA
#ifdef ZB_INCL_ZDA

#include <stdarg.h>

#ifndef Z_NO_ALLOC
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

#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#define zda_push(da, item)                                                       \
    do {                                                                         \
        if ((da)->len >= (da)->cap) {                                            \
            (da)->cap = (da)->cap == 0 ? ZDA_INIT_CAP : (da)->cap * 2;           \
            (da)->data = Z_REALLOC((da)->data, (da)->cap * sizeof(*(da)->data)); \
        }                                                                        \
        (da)->data[(da)->len++] = (item);                                        \
    } while (0)

#define zda_pushl(type, da, ...)                       \
    do {                                               \
        type item[] = { __VA_ARGS__ };                 \
        size_t count = sizeof(item) / sizeof(item[0]); \
        for (size_t i = 0; i < count; i++)             \
            zda_push(da, item[i]);                     \
    } while (0)

#define zda_pop(da) ((da)->len > 0 ? (da)->data[--(da)->len] : NULL)

#define zda_free(da)        \
    do {                    \
        Z_FREE((da)->data); \
        (da)->data = NULL;  \
        (da)->len = 0;      \
        (da)->cap = 0;      \
    } while (0)

#endif // ZB_INCL_ZDA

typedef enum zb_log_level {
    ERROR = 0,
    WARN,
    INFO
} zb_log_level;

typedef struct zb_artifacts {
    const char* out;
    const char* bld_dir;
    const char* obj_dir;
} zb_artifacts;

typedef struct zbinfo {
    uint32_t srcs_len;
    const char** srcs;
    uint32_t flags_len;
    const char** flags;
    const char* cc;
    zb_artifacts artifacts;
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
        .artifacts = {
            .out = "a.out",
            .bld_dir = "build",
            .obj_dir = "build/obj",
        },
        .debug = Z_TRUE,
        .log_level = INFO,
    };
    return b;
}

static void zb_info(zbinfo* b)
{
    printf("cc: %s\n", b->cc);
    printf("artifacts:\n");
    printf("  out: %s\n", b->artifacts.out);
    printf("  bld_dir: %s\n", b->artifacts.bld_dir);
    printf("  obj_dir: %s\n", b->artifacts.obj_dir);
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

// TODO: make compile object files
// TODO: make link object files
// TODO: only compile changed files
// TODO: make string builder
static void zb_build(zbinfo* b)
{
    zda(const char*) args = { 0 };
    char* output[1024] = { 0 };
    strcpy(output, b->artifacts.bld_dir);
    strcat(output, "/");
    strcat(output, b->artifacts.out);

    char outdir[1024] = { 0 };
    strcpy(outdir, "mkdir -p ");
    strcat(outdir, b->artifacts.bld_dir);
    int ret = system(outdir);
    if (ret != 0) {
        fprintf(stderr, "\nFailed to create output directory\n");
        exit(ret);
    }

    zda_pushl(char*, &args, b->cc, "-o", output);
    if (b->debug) {
        zda_push(&args, "-g");
    }
    for (uint32_t i = 0; i < b->srcs_len; i++) {
        zda_push(&args, b->srcs[i]);
    }

    for (uint32_t i = 0; i < b->flags_len; i++) {
        zda_push(&args, b->flags[i]);
    }

    if (b->log_level >= INFO) {
        printf("Compiling: ");
        for (uint32_t i = 0; i < args.len; i++) {
            printf("%s ", args.data[i]);
        }
        printf("\b\n");
    }

    zda_push(&args, NULL);

    // TODO: multithreading
    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(args.data[0], args.data) < 0) {
            if (b->log_level >= ERROR) {
                fprintf(stderr, "\n[ERROR] Failed to execute %s\n", args.data[0]);
                perror("execv");
            }
            exit(1);
        }
        exit(0);
    } else {
        int status = 0;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                if (b->log_level >= ERROR) {
                    fprintf(stderr, "\n[ERROR] Compilation failed\n");
                }
                exit(exit_status);
            }
        }
    }

    if (b->log_level >= INFO) {
        printf("Compiled: %s/%s\n", b->artifacts.bld_dir, b->artifacts.out);
    }
    zda_free(&args);
}

static void zb_run(zbinfo* b)
{
    int pid = fork();
    if (pid == 0) {
        char cmd[1024] = { 0 };
        sprintf(cmd, "%s/%s", b->artifacts.bld_dir, b->artifacts.out);
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
                    fprintf(stderr, "\n[ERROR] %s exited with status %d\n", b->artifacts.out, exit_status);
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
