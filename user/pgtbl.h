#ifndef PGTBL_H
#define PGTBL_H

#include "user/user.h"   // syscall numbers, printf, etc.

#define SUPERPGSIZE 0x200000  // 2 MiB
#define SUPERPGROUNDUP(a) (((a)+SUPERPGSIZE-1) & ~(SUPERPGSIZE-1))
#define SUPERPGROUNDDOWN(a) ((a) & ~(SUPERPGSIZE-1))

typedef unsigned long pte_t;

// User-space syscall wrappers
pte_t pgpte(uint64 va);
void kpgtbl(void);
int ugetpid(void);

#endif

