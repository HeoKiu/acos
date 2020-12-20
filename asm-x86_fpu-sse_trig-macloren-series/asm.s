.global my_sin
.intel_syntax noprefix
.text


my_sin:
    push ebp
    mov	ebp, esp

    sub esp, 8

    mov ecx, 100000
    movsd xmm3, [ebp+8]
    movsd xmm1, xmm3
    mov eax, 1
    movsd xmm0, xmm1

.Loop:

    inc eax # a += 1
    cvtsi2sd xmm2, eax # x2 = a
    divsd xmm1, xmm2 # curr/= x2

    inc eax
    cvtsi2sd xmm2, eax
    divsd xmm1, xmm2

    mov edx, -1
    cvtsi2sd xmm2, edx
    mulsd xmm1, xmm2 # curr *= -1

    mulsd xmm1, xmm3 # curr *= x
    mulsd xmm1, xmm3 # curr *= x

    addsd xmm0, xmm1 # res += curr

    loop .Loop

    movsd [ebp-8], xmm0
    fld qword ptr [ebp-8]

    leave
    ret