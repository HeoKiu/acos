    .global f
    .text
f:
    // r0 - A, r1 - B, r2 - C, r3 - x
    push {r4}
    mul r4, r0, r3
    mul r0, r4, r3
    mul r4, r1, r3
    add r0, r0, r4
    add r0, r0, r2
    pop {r4}
    bx lr
