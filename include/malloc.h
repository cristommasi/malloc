#ifndef MALLOC_H
#   define MALLOC_H

#define PUBLIC __attribute__((visibility("default")))

#include <stddef.h>

 // valgrind macros
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>

/**
 * @fn void *malloc(size_t size)
 * @brief Allocates size bytes and returns a pointer to the allocated memory or NULL on fail. 
 * 
 * @attention 
 * The memory is not initialized.
 * If size is 0, minimum chunk is still allocated.
 * 
 */
PUBLIC void     *malloc(size_t size);


/**
 * @fn void free(void *ptr)
 * @brief Frees the memory space pointed to by ptr, which must have been returned by a previous call to malloc() or realloc(). 
 * 
 * @attention 
 * If free(ptr) has already been called before undefined behavior occurs. 
 * If ptr is NULL, no operation is performed.
 */
PUBLIC void      free(void *ptr);

/**
 * @fn void *realloc(void *ptr, size_t size)
 * @brief Re-allocate the previously allocated block in PTR, making the new block SIZE bytes long.
 * 
 * @attention
 * If size was equal to 0, realloc() acts as free.
 * If realloc() fails, the original block is left untouched; it is not freed or moved.
 * 
 * The returned pointer may be the same as ptr if the allocation  was not moved, or different from ptr if the allocation was moved to a new address.
 * 
 * @return Pointer to the newly allocated memory, or NULL if the request failed.
 * 
 */
PUBLIC void     *realloc(void *ptr, size_t size);


/**
 * @fn void show_alloc_mem(void)
 * @brief Show allocated addresses and heap type.
 * 
 * @attention Function doesnt print anything if no heaps are found.
 * 
 */
PUBLIC void      show_alloc_mem(void);

/**
 * @fn void show_alloc_mem_ex(void)
 * @brief Show hex dump and ASCII representation of allocated zones.
 * 
 * @attention Function doesnt print anything if no heaps are found. 
 * 
 * @note M_SHOW_INFO may be set to show all heap memory.
 */
PUBLIC void      show_alloc_mem_ex(void);

/**
 * @brief General interface to tunable parameters.
 *
 * @verbatim
 * param, [values]:
 *
 * - M_SHOW_INFO, [M_SHOW_INUSE | M_SHOW_ALL]
 *
 * - M_CHECK_ACTION, [M_CHECK_SILENT | M_CHECK_PRINT | M_CHECK_ABORT | M_CHECK_PRINT_ABORT]
 * 
 * - M_PERTURB, [ 0-255 | M_PERTURB_NONE]
 *
 * - M_ZERO, [M_ZERO_INIT | M_ZERO_NONE]
 * @endverbatim
 * 
 * @return On success, 1.  On error, 0.
 * 
 * @note ENV_VARS may be set during initalization.
 * 
 * - MALLOC_SHOW_ | MALLOC_CHECK_ | MALLOC_PERTURB_ | MALLOC_ZERO_
 */
PUBLIC int       mallopt(int param, int value);



/**
 * @def M_SHOW_INFO
 * @brief mallopt() param controlling show_alloc_mem_ex() visibility.
 *
 * values:
 * \n
 * - M_SHOW_ALL   : show all memory in heap
 * \n
 * - M_SHOW_INUSE : show only allocated chunks (set default)
 * 
 */
#define M_SHOW_INFO        -9
#define M_SHOW_INUSE        0
#define M_SHOW_ALL          1


/**
 * @def M_CHECK_ACTION
 * @brief mallopt() param controlling how libft responds when various kinds of programming errors are detected.
 * 
 * Values:
 * \n
 * - M_CHECK_SILENT : Ignore errors
 * \n
 * - M_CHECK_PRINT  : Print errors to stderr
 * \n
 * - M_CHECK_ABORT  : Abort on error 
 * \n
 * - M_CHECK_PRINT_ABORT : Print error and abort (set default)
 */
#define M_CHECK_ACTION     -5
#define M_CHECK_SILENT      0
#define M_CHECK_PRINT       1
#define M_CHECK_ABORT       2
#define M_CHECK_PRINT_ABORT 3


/**
 * @def M_PERTURB
 * @brief mallopt() param controlling allocated memory perturbation.
 * \n
 *  - On alloc, bytes are initialized to the complement of the value in the least significant byte of value. 
 * \n
 *  - On free, the freed bytes are set to the least significant byte of value. 
 *
 * Values:
 * \n
 * - (1 - 255)      : Perturb with this byte pattern
 * \n
 * - M_PERTURB_NONE : No perturb (set default)
 * 
 */
#define M_PERTURB          -6
#define M_PERTURB_NONE      0


/**
 * @def M_ZERO
 * @brief mallopt() param controlling allocated memory initalization to 0.
 * 
 * Values:
 * \n
 * - M_ZERO_INIT      : Zero init
 * \n
 * - M_ZERO_NONE : No zero init (set default)
 * 
 * @note This action is performed after M_PERTURB if set.
 */
#define M_ZERO         -10
#define M_ZERO_INIT     1
#define M_ZERO_NONE     0

#endif
