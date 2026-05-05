#ifndef MALLOC_CONSTANTS_H
#   define MALLOC_CONSTANTS_H

 // size_t
#include <stdint.h>

 // getpagesize / sysconf(_SC_PAGESIZE) / write()
#include <unistd.h>


#if SIZE_MAX != 0xFFFFFFFFFFFFFFFFULL
#	error "size_t must be 8 bytes (64-bit platform required)"
#endif

 // SYSTEM DEFAULT PAGE_SIZE (4096)
#ifdef __APPLE__
#   define PAGE_SIZE (size_t)getpagesize()
#else
#   define PAGE_SIZE (size_t)sysconf(_SC_PAGESIZE)
#endif

 // SYSTEM DEFAULT PROT FLAGS
#define PROT_FLAGS (PROT_READ | PROT_WRITE)


 // SYSTEM DEFAULT MAP FLAGS
#ifdef __APPLE__ 
#   define MAP_FLAGS (MAP_PRIVATE | MAP_ANON)
#else
#   define MAP_FLAGS (MAP_PRIVATE | MAP_ANONYMOUS)
#endif


// NO FD FLAG
#define NO_FD -1

// NO OFFSET FLAG
#define NO_OFFSET 0

 // BLOCK ALIGNMENT MULTIPLES OF 8
#define ALIGNMENT (sizeof(size_t))

 // MACRO FN TO ALIGN
#define ALIGN(size) (((size) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))

#define HEAP_TYPE_COUNT 3

#define TINY_SMALL_BLOCK_MAX 128

 // max bytes for a tiny request
#define TINY_CHUNK_MAX 112

 // max bytes for a small request
#define SMALL_CHUNK_MAX 1008

#define LARGE_CHUNK_MIN 1009

 // 16384 - fits 128 tiny allocs
#define TINY_HEAP_SIZE (4 * PAGE_SIZE)

 // 131072 - fits 128 small allocs
#define SMALL_HEAP_SIZE (32 * PAGE_SIZE)

 // 16, 32, 48, 64, 80, 96, 112, 128
#define FASTBIN_MIN_CHUNK 16

 // 0, 1, 2, 3, 4, 5, 6, 7
#define FASTBIN_COUNT 64

 // MIN size to leave a chunk with 16 header + 16 data
#define MIN_TRIM 32

#define MIN_CHUNK_SIZE 32

#define HEX_DUMP_HEADER_TXT "Address\t\t    Hex bytes\t\t\t\t     ASCII\n"

// index 0 doesnt exist
#define BIN_IDX(size) (((size - FASTBIN_MIN_CHUNK) / ALIGNMENT))


 // masks

#define NO_FLAGS      0b0
#define IN_USE        0b001
#define IS_CIS        0b010
#define IS_LARGE      0b100
#define TS_FLAG_MASK  0b00000000000000000000000000000111U
#define TS_SIZE_MASK  0b11111111111111111111111111111000U 
#define L_FLAG_MASK   ((size_t)0b0000000000000000000000000000000000000000000000000000000000000111)
#define L_SIZE_MASK   ((size_t)0b1111111111111111111111111111111111111111111111111111111111111000)




 // _MALLOC_CHECK_
#define _MALLOC_CHECK_PARAM_            0x0
#define _M_CHECK_SILENT                 (uint8_t)0
#define _M_CHECK_PRINT                  (uint8_t)1
#define _M_CHECK_ABORT                  (uint8_t)2
#define _M_CHECK_PRINT_ABORT            (uint8_t)3
#define _M_CHECK_DEFAULT				(uint8_t)3


 // _MALLOC_PERTURB_
#define _MALLOC_PERTURB_PARAM_          0x1
#define FREE_PERTURB                    0
#define ALLOC_PERTURB                   1
#define _M_PERTURB_DEFAULT              (size_t)0

 // _MALLOC_ARENA_MAX_
#define _MALLOC_ARENA_MAX_PARAM_        0x2
#define _M_ARENA_MAX_DEFAULT            (uint32_t)0


 // _MALLOC_MMAP_THRESHOLD_
#define _MALLOC_MMAP_THRESHOLD_PARAM_   0x3
#define _M_MMAP_T_DEFAULT               (size_t)1009

 // mallopt()
#define M_PARAM_ERROR                   1
#define M_ARENA_MAX_EXCEEDED_ERROR     ((void*)-2)
#define F_MUMMAP_ERROR                 -1
#define F_NO_ERROR                      0
#define F_DOUBLE_FREE_ERROR             1
#define F_INV_PTR_ERROR                 2

 // show_alloc_mem_ex params
#define M_SHOW_INUSE 0
#define M_SHOW_INUSE_FREE 1
#define M_SHOW_ALL 2

#define M_PARAM_ERR_MSG "mallopt(): parameter out of bounds\n"
#define M_ARENA_MAX_EXCEEDED_MSG "malloc(): max number of arenas exceeded\n"
#define F_DOUBLE_FREE_MSG "free(): double free detected in tcache 2\n"
#define F_INV_PTR_MSG "free(): invalid pointer\n"
#define F_MUNMAP_MSG "free(): munmap failed!\n"
#define F_ABORT_MSG "abort()\n"

#endif