#ifndef MALLOC_H
 #define MALLOC_H

#include <sys/mman.h> // mmap(2)
#include <sys/time.h> // munmap(2)
#include <unistd.h> // getpagesize / sysconf(_SC_PAGESIZE)
#include <sys/resource.h> //getrlimit(2)
#include <pthread.h> // thread functions
#include "libft.h" // libft

#include <stdio.h>


#endif