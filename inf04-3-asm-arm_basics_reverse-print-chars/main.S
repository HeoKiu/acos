.extern fgetc, fputc, realloc, stdin, stdout

.global main
.text


// r4 - buffer size
// r5 - buffer ptr
// r6 - ptr to next sym in buffer

main:
    push {r4, r5, r6, lr}

create_buffer:
    mov r0, #0
    mov r4, #8
    mov r1, r4
    bl realloc
    mov r5, r0

readsym:
    ldr r0, =stdin
    ldr r0, [r0]
    bl fgetc
    // if (sym == EOF) goto print_buffer
    cmp r0, #-1
    beq print_buffer

    // write to buffer
    str r0, [r5, r6]

    add r6, #4

    // if(curr_ptr < buffer_size)
    cmp r6, r4
    blt skip_double_buffer
double_buffer:
    mov r0, r5
    mov r3, #2
    mul r4, r3
    mov r1, r4
    bl realloc
    mov r5, r0
skip_double_buffer:
    b readsym

print_buffer:
    subs r6, #4
    ldr r0, [r5, r6]

    ldr r1, =stdout
    ldr r1, [r1]
    bl fputc

    cmp r6, #0
    beq exit_main
    b print_buffer


exit_main:
free_buffer:
    mov r0, r5
    bl free

    pop {r4, r5, r6, lr}
    mov r0, #0
    bx lr

