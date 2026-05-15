#ifndef MALLOC_H
# define MALLOC_H

# define PUBLIC __attribute__((visibility("default")))

# include <stddef.h>

// Allocate SIZE bytes of memory.
PUBLIC void     *malloc(size_t size);
// Free a block allocated by malloc or realloc.
PUBLIC void      free(void *ptr);
//Re-allocate the previously allocated block in PTR, making the new block SIZE bytes long.
PUBLIC void     *realloc(void *ptr, size_t size);
// Show allocated addresses and heap type.
PUBLIC void      show_alloc_mem(void);
// Show hex dump and ASCII representation of allocated zones.
// MALLOC_SHOW_INFO may be set to view free zones.
PUBLIC void      show_alloc_mem_ex(void);
// General interface to tunable parameters.
// 
// MALLOC_CHECK: M_CHECK_SILENT | M_CHECK_PRINT | M_CHECK_ABORT (uint8_t)
// MALLOC_PERTURB: M_PERTURB_NONE - UCHAR_MAX (uint8_t)
// MALLOC_ARENA_MAX: M_ARENA_MAX_DEFAULT - UINT_MAX (uint32_t)
// MALLOC_SHOW_INFO: M_SHOW_INUSE | M_SHOW_ALL
PUBLIC int       mallopt(int param, int value);

 // MALLOC_ZERO
#define MALLOC_SHOW_INFO    0x0
#define M_SHOW_INUSE        (uint8_t)0
#define M_SHOW_ALL          (uint8_t)1
#define M_SHOW_DEFAULT      (uint8_t)0

 // _MALLOC_CHECK_
#define MALLOC_CHECK        0x1
#define M_CHECK_SILENT     (uint8_t)0
#define M_CHECK_PRINT      (uint8_t)1
#define M_CHECK_ABORT      (uint8_t)2
#define M_CHECK_DEFAULT    (uint8_t)3

 // _MALLOC_PERTURB_
#define MALLOC_PERTURB      0x2
#define M_PERTURB_NONE   (uint8_t)0

 // _MALLOC_ARENA_MAX_
#define MALLOC_ARENA_MAX     0x3
#define M_ARENA_MAX_DEFAULT  (uint32_t)65530

#endif