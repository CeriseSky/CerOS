#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stddef.h>

// general memory management tools

void memset(void *s, char c, size_t n);
void memcpy(void *restrict dst, void *restrict src, size_t n);

#endif

