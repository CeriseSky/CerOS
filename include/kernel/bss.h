#ifndef _BSS_H_
#define _BSS_H_

// tools for manipulating the uninitialised memory region of the kernel

extern void *_bss_start;
extern void *_bss_end;

void bss_clear();

#endif

