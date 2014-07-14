
#include "stddef.h"

#define STR_HELPER(X) #X
#define STR(X) STR_HELPER(X)

#define BYTE_ALIGN 2
#define BYTE_ALIGN_MASK 3

#define HEAPSZ 0x8000

#if 0
#ifndef uint8_t 
typedef unsigned char   uint8_t;
#endif

#ifndef size_t 
typedef long unsigned int   size_t;
#endif
#endif

void* malloc1(size_t sz);
char* strdup1(char *str);

