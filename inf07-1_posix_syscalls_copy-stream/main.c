#include <sys/syscall.h>

// run with: gcc -nostdlib main.c syscall.s && ./a.out

extern long syscall(long number, ...);

void _start() {
    char sym;

    while (syscall(SYS_read, 0, &sym, 1)) {
        syscall(SYS_write, 1, &sym, 1);
    }

    syscall(SYS_exit_group, 0);
}

