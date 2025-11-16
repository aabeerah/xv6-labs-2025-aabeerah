// user/pgtbltest.c
#include "user/user.h"
#include "kernel/types.h"
#include "kernel/param.h"

#define SUPERPGSIZE 8192
#define SUPERPGROUNDUP(x) (((x) + SUPERPGSIZE - 1) & ~(SUPERPGSIZE-1))
#define SUPERPGROUNDDOWN(x) ((x) & ~(SUPERPGSIZE-1))
#define SZ (8 * SUPERPGSIZE)

char *testname = "???";

void err(char *why) {
    printf("pgtbltest: %s failed: %s, pid=%d\n", testname, why, getpid());
    exit(1);
}

// syscall wrappers (must exist in usys.S + kernel)
extern int pgpte(uint64 va);
extern void kpgtbl(void);
extern int ugetpid(void);

void print_pte(uint64 va) {
    int pte = pgpte(va);  // syscall
    uint64 pa = (pte & ~0x3FF); // PTE2PA
    uint64 perm = (pte & 0x3FF); // permission bits
    printf("va 0x%lx pte 0x%x pa 0x%lx perm 0x%lx\n", va, pte, pa, perm);
}

void print_pgtbl() {
    printf("print_pgtbl starting\n");
    for (uint64 i = 0; i < 10; i++) {
        print_pte(i * PGSIZE);
    }
    uint64 top = MAXVA / PGSIZE;
    for (uint64 i = top - 10; i < top; i++) {
        print_pte(i * PGSIZE);
    }
    printf("print_pgtbl: OK\n");
}

void ugetpid_test() {
    printf("ugetpid_test starting\n");
    testname = "ugetpid_test";

    for (int i = 0; i < 64; i++) {
        int ret = fork();
        if (ret != 0) {
            int status;
            wait(&status);
            if (status != 0) exit(1);
            continue;
        }
        if (getpid() != ugetpid())
            err("missmatched PID");
        exit(0);
    }
    printf("ugetpid_test: OK\n");
}

void print_kpgtbl() {
    printf("print_kpgtbl starting\n");
    kpgtbl();  // syscall
    printf("print_kpgtbl: OK\n");
}

void supercheck(char *end) {
    uint64 last_pte = 0;
    uint64 a = (uint64) end;
    uint64 s = SUPERPGROUNDUP(a);

    for (; a < s; a += PGSIZE) {
        int pte = pgpte(a);
        if (pte == 0) err("no pte");
    }

    for (uint64 p = s; p < s + 512 * PGSIZE; p += PGSIZE) {
        int pte = pgpte(p);
        if (pte == 0) err("no pte");
        if (last_pte != 0 && pte != last_pte) err("pte different");
        if ((pte & 0x1) == 0 || (pte & 0x2) == 0 || (pte & 0x4) == 0) err("pte wrong");
        last_pte = pte;
    }

    for (int i = 0; i < 512 * PGSIZE; i += PGSIZE) {
        *(int *)(s + i) = i;
    }

    for (int i = 0; i < 512 * PGSIZE; i += PGSIZE) {
        if (*(int *)(s + i) != i) err("wrong value");
    }
}

void superpg_fork() {
    int pid;
    printf("superpg_fork starting\n");
    testname = "superpg_fork";

    char *end = sbrk(SZ);
    if (end == 0 || end == (char *)-1) err("sbrk failed");

    supercheck(end);

    if ((pid = fork()) < 0) err("fork");
    else if (pid == 0) {
        supercheck(end);
        exit(0);
    } else {
        int status;
        wait(&status);
        if (status != 0) exit(1);
    }

    // free super pages
    sbrk(-SZ);
    if ((pid = fork()) < 0) err("fork");
    else if (pid == 0) {
        *(end + 1) = '9';  // should page-fault
        exit(0);
    } else {
        int status;
        wait(&status);
        if (status == 0) err("child referenced freed memory");
    }
    printf("superpg_fork: OK\n");
}

void superpg_free() {
    int pid;
    printf("superpg_free starting\n");
    testname = "superpg_free";

    char *end = sbrk(SZ);
    if (end == 0 || end == (char *)-1) err("sbrk failed");

    char *a = sbrk(0);
    uint64 s = SUPERPGROUNDDOWN((uint64)a);
    sbrk(-((uint64)a - s));
    a = sbrk(0);

    int pte1 = pgpte((uint64)(a - PGSIZE));
    int pte2 = pgpte((uint64)(a - 2 * PGSIZE));
    if (pte1 != pte2) err("not a super page");

    *(a - PGSIZE + 1) = '8';
    *(a - 2 * PGSIZE + 1) = '9';

    sbrk(-PGSIZE);
    a = sbrk(0);
    if (*(a - PGSIZE + 1) != '9') err("lost content after free");

    if ((pid = fork()) < 0) err("fork");
    else if (pid == 0) {
        *(a + 1);  // should page-fault
        exit(0);
    } else {
        int status;
        wait(&status);
        if (status == 0) err("child referenced freed memory");
    }

    int pte3 = pgpte((uint64)a);
    if (pte3 != 0) err("pte for freed memory is valid");

    printf("superpg_free: OK\n");
}

int main(int argc, char *argv[]) {
    print_pgtbl();
    ugetpid_test();
    print_kpgtbl();
    superpg_fork();
    superpg_free();
    printf("pgtbltest: all tests succeeded\n");
    exit(0);
}

