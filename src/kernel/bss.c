#include <bss.h>
#include <memory.h>

void bss_clear() {
  memset( (void *)&_bss_start, 0, &_bss_end - &_bss_start);
}

