    .global f
    .text
f:
    // r0 - A, r1 - B, r2 - C, r3 - x
    mul r0, r3
    mul r0, r3
    mul r1, r3
    add r0, r1
    add r0, r2

    bx lr
