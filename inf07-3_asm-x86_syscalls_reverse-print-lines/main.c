#include <stdio.h>
#include <asm/unistd.h>


int main() {
    printf("Hello, World!\n");
    return 0;
}
mov rax, 1
mov rdi, 1
mov rsi, hello
mov rdx, 5
syscall

mov rax, 60
syscall