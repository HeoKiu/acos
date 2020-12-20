.global dot_product
.intel_syntax noprefix
.text

dot_product:
    push ebp
    mov	ebp, esp

    sub esp, 4

    push esi
    push ebx
    push edi

    mov ecx, [ebp+8]
    sub ecx, 4
    mov esi, [ebp+12]
    mov ebx, [ebp+16]
    mov eax, 0

    cvtsi2ss xmm3, eax

Loop:
    cmp eax, ecx
    jg skip_loop4

    movups xmm0, [esi+eax*4]
    movups xmm1, [ebx+eax*4]

    dpps xmm0, xmm1, 0b11110001

    addss xmm3, xmm0

    add eax, 4
    jmp Loop

skip_loop4:
    add ecx, 4

loop1:
    cmp eax, ecx
    jge skip_loop
    movss xmm0, [esi+eax*4]
    movss xmm1, [ebx+eax*4]

    mulss xmm0, xmm1
    addss xmm3, xmm0

    inc eax
    jmp loop1
skip_loop:

    pop edi
    pop ebx
    pop esi

    movss [ebp-4], xmm3
    fld dword ptr [ebp-4]

    leave
    ret
