#include "pgtbl.h"
#include "user/user.h"

pte_t pgpte(uint64 va) {
    return syscall(SYS_pgpte, va);
}

void kpgtbl(void) {
    syscall(SYS_kpgtbl);
}

int ugetpid(void) {
    return syscall(SYS_ugetpid);
}

