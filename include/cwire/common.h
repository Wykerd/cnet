#ifndef CWR_COMMON_H
#define CWR_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <uv.h>

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

typedef struct cwr_buf_s {
    char* base;
    size_t len;
    size_t size;
} cwr_buf_t;

/* Malloc function implementation is same as the one used in QuickJS which is also MIT licensed */
typedef struct cwr_malloc_state_s {
    size_t malloc_count;
    size_t malloc_size;
    size_t malloc_limit;
    void *data; 
} cwr_malloc_state_t;

typedef struct cwr_mallloc_funcs_s {
    void *(*cwr_malloc)(cwr_malloc_state_t *, size_t);
    void (*cwr_free)(cwr_malloc_state_t *, void *);
    void *(*cwr_realloc)(cwr_malloc_state_t *, void *, size_t);
    size_t (*cwr_malloc_usable_size)(const void *ptr);
} cwr_malloc_funcs_t;

typedef struct cwr_malloc_ctx_s {
    cwr_malloc_funcs_t mf;
    cwr_malloc_state_t ms;
} cwr_malloc_ctx_t;

typedef enum cwr_err {
    CWR_E_INTERNAL,
    CWR_E_UV
} cwr_err_t;

typedef enum cwr_intr_err {
    CWR_E_INTERNAL_OK = 0,
    CWR_E_INTERNAL_OOM,
    CWR_E_INTERNAL_URLPARSE
} cwr_intr_err_t;

#define DEF_CWR_LINK_IO_SIGNATURE(classname, type) \
    typedef int (*cwr_ ## classname ## _io)(type *, const void *, size_t)

#define DEF_CWR_LINK_CB_SIGNATURE(classname, type) \
    typedef void (*cwr_ ## classname ## _cb)(type *)

#define DEF_CWR_LINK_CLS(classname, parent) \
    typedef struct cwr_ ## classname ## _s cwr_ ## classname ## _t;     \
    DEF_CWR_LINK_IO_SIGNATURE(classname, parent);                       \
    DEF_CWR_LINK_CB_SIGNATURE(classname, parent);                       \
    struct cwr_ ## classname ## _s {                                    \
        cwr_ ## classname ## _io reader;                                \
        cwr_ ## classname ## _io writer;                                \
        cwr_ ## classname ## _cb on_error;                              \
        cwr_ ## classname ## _io on_read;                               \
        cwr_ ## classname ## _io on_write;                              \
        cwr_err_t err_type;                                             \
        ssize_t err_code;                                               \
    }

DEF_CWR_LINK_CLS(link, void);

void cwr_malloc_ctx_new_ex (cwr_malloc_ctx_t *ctx, const cwr_malloc_funcs_t *mf);
void cwr_malloc_ctx_new (cwr_malloc_ctx_t *ctx);
int cwr_malloc_ctx_set_limit (cwr_malloc_ctx_t *ctx, size_t limit);
void cwr_malloc_ctx_dump_leaks (cwr_malloc_ctx_t *ctx);
void *cwr_malloc (cwr_malloc_ctx_t *ctx, size_t size);
void cwr_free (cwr_malloc_ctx_t *ctx, void *ptr);
void *cwr_realloc (cwr_malloc_ctx_t *ctx, void *ptr, size_t size);
void *cwr_mallocz (cwr_malloc_ctx_t *ctx, size_t size);

#endif