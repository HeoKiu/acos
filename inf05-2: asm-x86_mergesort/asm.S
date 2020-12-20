.global mergesort, malloc, free
.intel_syntax noprefix
.text

// Аргументы: rdi, rsi, rdx, rcx, r8, r9
// from - rdi, to - rsi, in - rdx, out - rcx

mergesort:
    push rbp
    mov rbp, rsp
    push rdi
    push rsi
    push rcx
    push rdx

    cmp rdi, rsi // if(from >= to) exit
    jl split_recursive
    jg exit_merge
    // out[from] = in[from]
    mov eax, [rdx+rdi*4]
    mov [rcx+rdi*4], eax
    jmp exit_merge


split_recursive:

    add rsi, rdi // to += from
    SHR rsi // to /= 2

    call mergesort

    mov rdi, rsi // from = to
    inc rdi // from ++
    mov rsi, [rbp-16] // to = origin to
    call mergesort
    mov rdi, [rbp-8] // from = original from

    call merge__


exit_merge:
    pop rdx
    pop rcx
    pop rsi
    pop rdi

    leave
    ret


// from - rdi, to - rsi, in - rdx, out - rcx
merge:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push r9
    push rdx
    push rcx

    // r9 = (from + to) / 2
    mov r9, rdi
    add r9, rsi
    SHR r9

    mov rax, rsi
    sub rax, rdi
    inc rax
    mov ebx, 4
    imul ebx

    mov rcx, rax
    mov rcx, 8
    call malloc



    call free

    push rcx
    pop rdx
    pop r9
    pop rbx
    pop rax

    //call free
    leave
    ret


get_value_from_asm:
    push rbp
    mov rbp, rsp

    // rcx rdx r8 r9
    mov rax, rsi

    leave
    ret


merge__:
.LFB6:
        .cfi_startproc
        endbr64
        push    rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        mov     rbp, rsp
        .cfi_def_cfa_register 6
        push    r15
        push    r14
        push    r13
        push    r12
        sub     rsp, 112
        .cfi_offset 15, -24
        .cfi_offset 14, -32
        .cfi_offset 13, -40
        .cfi_offset 12, -48
        mov     DWORD PTR -116[rbp], edi
        mov     DWORD PTR -120[rbp], esi
        mov     QWORD PTR -128[rbp], rdx
        mov     QWORD PTR -136[rbp], rcx
        mov     rax, QWORD PTR fs:40
        mov     QWORD PTR -40[rbp], rax
        xor     eax, eax
        mov     rax, rsp
        mov     rsi, rax
        mov     eax, DWORD PTR -116[rbp]
        mov     DWORD PTR -92[rbp], eax
        mov     eax, DWORD PTR -120[rbp]
        mov     DWORD PTR -88[rbp], eax
        mov     edx, DWORD PTR -92[rbp]
        mov     eax, DWORD PTR -88[rbp]
        add     eax, edx
        mov     edx, eax
        shr     edx, 31
        add     eax, edx
        sar     eax
        mov     DWORD PTR -84[rbp], eax
        mov     eax, DWORD PTR -84[rbp]
        sub     eax, DWORD PTR -92[rbp]
        add     eax, 1
        mov     DWORD PTR -80[rbp], eax
        mov     eax, DWORD PTR -88[rbp]
        sub     eax, DWORD PTR -84[rbp]
        mov     DWORD PTR -76[rbp], eax
        mov     eax, DWORD PTR -80[rbp]
        movsx   rdx, eax
        sub     rdx, 1
        mov     QWORD PTR -72[rbp], rdx
        movsx   rdx, eax
        mov     r14, rdx
        mov     r15d, 0
        movsx   rdx, eax
        mov     r12, rdx
        mov     r13d, 0
        cdqe
        lea     rdx, 0[0+rax*4]
        mov     eax, 16
        sub     rax, 1
        add     rax, rdx
        mov     edi, 16
        mov     edx, 0
        div     rdi
        imul    rax, rax, 16
        mov     rdx, rax
        and     rdx, -4096
        mov     rdi, rsp
        sub     rdi, rdx
        mov     rdx, rdi
.L2:
        cmp     rsp, rdx
        je      .L3
        sub     rsp, 4096
        or      QWORD PTR 4088[rsp], 0
        jmp     .L2
.L3:
        mov     rdx, rax
        and     edx, 4095
        sub     rsp, rdx
        mov     rdx, rax
        and     edx, 4095
        test    rdx, rdx
        je      .L4
        and     eax, 4095
        sub     rax, 8
        add     rax, rsp
        or      QWORD PTR [rax], 0
.L4:
        mov     rax, rsp
        add     rax, 3
        shr     rax, 2
        sal     rax, 2
        mov     QWORD PTR -64[rbp], rax
        mov     eax, DWORD PTR -76[rbp]
        movsx   rdx, eax
        sub     rdx, 1
        mov     QWORD PTR -56[rbp], rdx
        movsx   rdx, eax
        mov     r10, rdx
        mov     r11d, 0
        movsx   rdx, eax
        mov     r8, rdx
        mov     r9d, 0
        cdqe
        lea     rdx, 0[0+rax*4]
        mov     eax, 16
        sub     rax, 1
        add     rax, rdx
        mov     edi, 16
        mov     edx, 0
        div     rdi
        imul    rax, rax, 16
        mov     rdx, rax
        and     rdx, -4096
        mov     rdi, rsp
        sub     rdi, rdx
        mov     rdx, rdi
.L5:
        cmp     rsp, rdx
        je      .L6
        sub     rsp, 4096
        or      QWORD PTR 4088[rsp], 0
        jmp     .L5
.L6:
        mov     rdx, rax
        and     edx, 4095
        sub     rsp, rdx
        mov     rdx, rax
        and     edx, 4095
        test    rdx, rdx
        je      .L7
        and     eax, 4095
        sub     rax, 8
        add     rax, rsp
        or      QWORD PTR [rax], 0
.L7:
        mov     rax, rsp
        add     rax, 3
        shr     rax, 2
        sal     rax, 2
        mov     QWORD PTR -48[rbp], rax
        mov     DWORD PTR -96[rbp], 0
        jmp     .L8
.L9:
        mov     edx, DWORD PTR -92[rbp]
        mov     eax, DWORD PTR -96[rbp]
        add     eax, edx
        cdqe
        lea     rdx, 0[0+rax*4]
        mov     rax, QWORD PTR -136[rbp]
        add     rax, rdx
        mov     ecx, DWORD PTR [rax]
        mov     rax, QWORD PTR -64[rbp]
        mov     edx, DWORD PTR -96[rbp]
        movsx   rdx, edx
        mov     DWORD PTR [rax+rdx*4], ecx
        add     DWORD PTR -96[rbp], 1
.L8:
        mov     eax, DWORD PTR -96[rbp]
        cmp     eax, DWORD PTR -80[rbp]
        jl      .L9
        mov     DWORD PTR -100[rbp], 0
        jmp     .L10
.L11:
        mov     edx, DWORD PTR -84[rbp]
        mov     eax, DWORD PTR -100[rbp]
        add     eax, edx
        cdqe
        add     rax, 1
        lea     rdx, 0[0+rax*4]
        mov     rax, QWORD PTR -136[rbp]
        add     rax, rdx
        mov     ecx, DWORD PTR [rax]
        mov     rax, QWORD PTR -48[rbp]
        mov     edx, DWORD PTR -100[rbp]
        movsx   rdx, edx
        mov     DWORD PTR [rax+rdx*4], ecx
        add     DWORD PTR -100[rbp], 1
.L10:
        mov     eax, DWORD PTR -100[rbp]
        cmp     eax, DWORD PTR -76[rbp]
        jl      .L11
        mov     DWORD PTR -96[rbp], 0
        mov     DWORD PTR -100[rbp], 0
        mov     eax, DWORD PTR -92[rbp]
        mov     DWORD PTR -104[rbp], eax
        jmp     .L12
.L16:
        mov     rax, QWORD PTR -64[rbp]
        mov     edx, DWORD PTR -96[rbp]
        movsx   rdx, edx
        mov     ecx, DWORD PTR [rax+rdx*4]
        mov     rax, QWORD PTR -48[rbp]
        mov     edx, DWORD PTR -100[rbp]
        movsx   rdx, edx
        mov     eax, DWORD PTR [rax+rdx*4]
        cmp     ecx, eax
        jge     .L13
        mov     eax, DWORD PTR -96[rbp]
        lea     edx, 1[rax]
        mov     DWORD PTR -96[rbp], edx
        mov     edx, DWORD PTR -104[rbp]
        movsx   rdx, edx
        lea     rcx, 0[0+rdx*4]
        mov     rdx, QWORD PTR -136[rbp]
        add     rcx, rdx
        mov     rdx, QWORD PTR -64[rbp]
        cdqe
        mov     eax, DWORD PTR [rdx+rax*4]
        mov     DWORD PTR [rcx], eax
        jmp     .L14
.L13:
        mov     eax, DWORD PTR -100[rbp]
        lea     edx, 1[rax]
        mov     DWORD PTR -100[rbp], edx
        mov     edx, DWORD PTR -104[rbp]
        movsx   rdx, edx
        lea     rcx, 0[0+rdx*4]
        mov     rdx, QWORD PTR -136[rbp]
        add     rcx, rdx
        mov     rdx, QWORD PTR -48[rbp]
        cdqe
        mov     eax, DWORD PTR [rdx+rax*4]
        mov     DWORD PTR [rcx], eax
.L14:
        add     DWORD PTR -104[rbp], 1
.L12:
        mov     eax, DWORD PTR -96[rbp]
        cmp     eax, DWORD PTR -80[rbp]
        jge     .L17
        mov     eax, DWORD PTR -100[rbp]
        cmp     eax, DWORD PTR -76[rbp]
        jl      .L16
        jmp     .L17
.L18:
        mov     eax, DWORD PTR -96[rbp]
        lea     edx, 1[rax]
        mov     DWORD PTR -96[rbp], edx
        mov     edx, DWORD PTR -104[rbp]
        lea     ecx, 1[rdx]
        mov     DWORD PTR -104[rbp], ecx
        movsx   rdx, edx
        lea     rcx, 0[0+rdx*4]
        mov     rdx, QWORD PTR -136[rbp]
        add     rcx, rdx
        mov     rdx, QWORD PTR -64[rbp]
        cdqe
        mov     eax, DWORD PTR [rdx+rax*4]
        mov     DWORD PTR [rcx], eax
.L17:
        mov     eax, DWORD PTR -96[rbp]
        cmp     eax, DWORD PTR -80[rbp]
        jl      .L18
        jmp     .L19
.L20:
        mov     eax, DWORD PTR -100[rbp]
        lea     edx, 1[rax]
        mov     DWORD PTR -100[rbp], edx
        mov     edx, DWORD PTR -104[rbp]
        lea     ecx, 1[rdx]
        mov     DWORD PTR -104[rbp], ecx
        movsx   rdx, edx
        lea     rcx, 0[0+rdx*4]
        mov     rdx, QWORD PTR -136[rbp]
        add     rcx, rdx
        mov     rdx, QWORD PTR -48[rbp]
        cdqe
        mov     eax, DWORD PTR [rdx+rax*4]
        mov     DWORD PTR [rcx], eax
.L19:
        mov     eax, DWORD PTR -100[rbp]
        cmp     eax, DWORD PTR -76[rbp]
        jl      .L20
        mov     rsp, rsi
        nop
        mov     rax, QWORD PTR -40[rbp]
        xor     rax, QWORD PTR fs:40
        je      .L21
        call    __stack_chk_fail@PLT
.L21:
        lea     rsp, -32[rbp]
        pop     r12
        pop     r13
        pop     r14
        pop     r15
        pop     rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc