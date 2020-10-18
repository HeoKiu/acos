    .global summ
    .text

summ:
    ldr r3, [r2]
    add r2, r2, #4
    add r0, r0, r3
    sub r1, r1, #1
    cmp r1, #0
    bgt summ
    bx lr
