// Compile with: gcc -nostdlib main.S
// gcc -nostdlib main.s && ./a.out < input.txt


    .intel_syntax noprefix
    .global _start
    .text


_start:
    mov rax, 12
    mov rdi, 0
    syscall
    mov rbp, rax
    mov rsp, rax

    mov rax, 12
    lea rdi, [rbp+40000000]
    syscall

    mov rax, 10
    mov [rbp], rax # *rbp = \n
    inc rsp

read_begin:
    mov rax, 0 # SYS_READ
    mov rdi, 0 # stdin
    mov rsi, rsp
    mov rdx, 10000 # read 10000 symbols
    syscall
    cmp rax, 0
    jl strip_spaces
    add rsp, rax
    cmp rax, 0
    jnz read_begin

    # Skip spaces and new lines at the end of file
strip_spaces:
    dec rsp
    cmp rsp, rbp
    jle exit
    cmpb [rsp], 10
    jz strip_spaces
    cmpb [rsp], 32
    jz strip_spaces

    inc rsp

    mov rax, 10
    mov [rsp], rax # *rbp = \n

    mov rbx, rsp

    #jmp exit

find_newline_begin:
    dec rbx # --rbx
    cmpb [rbx], 10 # if rbx == '\n'
    jnz find_newline_begin

    mov rcx, rsp
    sub rcx, rbx

    mov rax, 1 # SYS_WRITE
    mov rdi, 1 # stdout
    lea rsi, [rbx + 1] # cout << *(rb)
    mov rdx, rcx # count = rcx
    syscall

    mov rsp, rbx
    cmp rbx, rbp
    jg find_newline_begin
exit:
    mov rax, 60
    mov rdi, 0
    syscall
