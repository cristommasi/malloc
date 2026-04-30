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

#define F_MUMMAP_ERROR -1

#define F_NO_ERROR 0

#define F_DOUBLE_FREE_ERROR 1

#define F_INV_PTR_ERROR 2


#define F_DOUBLE_FREE_MSG "free(): double free detected in tcache 2\n"

#define F_INV_PTR_MSG "free(): invalid pointer\n"

#define F_MUNMAP_MSG "free(): munmap failed!\n"

#define F_ABORT_MSG "abort()\n"

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


// index 0 doesnt exist
#define BIN_IDX(size) (((size - FASTBIN_MIN_CHUNK) / ALIGNMENT))


#define BIN_LAST 0


 // masks
#define IN_USE        0b001

#define IS_CIS        0b010

#define IS_LARGE      0b100

#define TS_FLAG_MASK  0b00000000000000000000000000000111U

#define TS_SIZE_MASK  0b11111111111111111111111111111000U 

#define L_FLAG_MASK   ((size_t)0b0000000000000000000000000000000000000000000000000000000000000111)

#define L_SIZE_MASK   ((size_t)0b1111111111111111111111111111111111111111111111111111111111111000)


 // default scribble bytes
#define DEFAULT_ALLOC_SCRIBBLE (char)0xAA

#define DEFAULT_FREE_SCRIBBLE (char)0x55


#endif