#include <sys/syscall.h>

// run with: gcc -nostdlib main.c syscall.s && ./a.out

extern long syscall(long number, ...);


void _start() {
    const char Hello[] = "Hello, World!";

    syscall(SYS_write, 1, Hello, sizeof(Hello) - 1);
    syscall(SYS_exit_group, 0);
}

