    .global summ, everyday795
    .intel_syntax noprefix

    .text

summ:
    push ebx
    mov ecx, N

.Loop:
    mov ebx, ecx
    dec ebx
    mov edx, A
    mov eax, [edx+4*ebx]
    mov edx, B
    add eax, [edx+4*ebx]
    mov edx, R
    mov [edx+4*ebx], eax
    loop .Loop

    pop ebx
    ret


everyday795:
    push ebp
    mov ebp, esp
    sub esp, 4
    push ebx


    mov ebx, ebp
    sub ebx, 4
    push ebx
    mov ebx, offset scanf_format_str
    push ebx
    call scanf
    add esp, 8

    mov ebx, [ebp-4]
    imul ebx, [ebp+8]
    add ebx, [ebp+12]

    // printf
    push ebx
    mov ebx, offset printf_format_str
    push ebx
    call printf
    add esp, 8

    // correct exit
    pop ebx
    mov esp, ebp
    pop ebp
    ret


printf_format_str:
    .string "%d\n"

scanf_format_str:
    .string "%d"