#ifndef MALLOC_H
#   define MALLOC_H


#define _GNU_SOURCE
 // mmap(2)
#include <sys/mman.h>
 // munmap(2)
#include <sys/time.h>
 // getpagesize / sysconf(_SC_PAGESIZE) / write()
#include <unistd.h>
 //getrlimit(2)
#include <sys/resource.h>
 // thread functions
#include <pthread.h>
 // libft
#include "./libft/includes/libft.h"
 // printf
#include <stdio.h> 


 // SYSTEM DEFAULT PAGE_SIZE
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

#define NO_FD -1

#define NO_OFFSET 0



// void ft_free(void *ptr);
// void *ft_malloc(size_t size);
// void *ft_realloc(void *ptr, size_t size);
// void    show_alloc_mem(void);

#endif