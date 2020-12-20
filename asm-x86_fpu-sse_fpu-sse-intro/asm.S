.global calc, vector_sum
.intel_syntax noprefix
.text


calc:
    push ebp
    mov	ebp, esp

    sub esp, 8

    movsd xmm0, [ebp+16]
    addsd xmm0, [ebp+8]

    cvtsi2sd xmm1, [ebp+32]
    addsd xmm1, [ebp+24]

    divsd xmm0, xmm1
    movsd [ebp-8], xmm0
    fld qword ptr [ebp-8]

    leave
    ret


vector_sum:
    push ebp
    mov	ebp, esp

    push esi
    push edi
    push ebx

    mov ecx, [ebp+8]
    mov esi, [ebp+12]
    mov edi, [ebp+16]
    mov ebx, [ebp+20]
    mov eax, 0

Loop:
    // mov aligned packed single
    movaps xmm0, [esi+eax*4]
    movaps xmm1, [edi+eax*4]
    addps xmm0, xmm1

    movaps [ebx+eax*4], xmm0
    add eax, 4
    cmp eax, ecx
    jl Loop

    pop ebx
    pop edi
    pop esi

    leave
    ret

