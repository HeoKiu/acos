.global summ
.intel_syntax noprefix
.text

summ:
    push ebp
    mov	ebp, esp

    push ebx
    push edi

    // edi = R, eax = A, ebx = B
    mov edi, [ebp+20]
    mov eax, [ebp+16]
    mov ebx, [ebp+12]

    mov ecx, [ebp+8]
.Loop:
    mov edx, [eax]
    add edx, [ebx]
    mov [edi], edx

    add eax, 4
    add ebx, 4
    add edi, 4

    loop .Loop

    pop edi
    pop ebx

    leave
    ret
