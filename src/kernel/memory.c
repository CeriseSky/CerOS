#include "memory.h"
#include <stdint.h>

void memset(void *s, char c, size_t n) {
  for(size_t i = 0; i < n; i++, *(char *)s++ = c);
}

void memcpy(void *restrict dst, void *restrict src, size_t n) {
  for(size_t i = 0; i < n; i++)
    ((uint8_t *)dst)[i] = ((uint8_t *)src)[i];
}

