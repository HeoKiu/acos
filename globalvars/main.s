.data
    .global R
    R:
        .space 4

.text
.global calculate

calculate:
    // r0, r1 = A, B
    ldr r0, .Lexterns
    ldr r0, [r0]
    ldr r1, .Lexterns+4
    ldr r1, [r1]
    // r2 = A*B
    mul r2, r0, r1
    // r0, r1 = C, D
    ldr r0, .Lexterns+8
    ldr r0, [r0]
    ldr r1, .Lexterns+12
    ldr r1, [r1]
    // r3 = C*D
    mul r3, r0, r1
    add r0, r2, r3
    ldr r1, .Lexterns+16
    str r0, [r1]
    bx lr


.Lexterns:
    .word A
    .word B
    .word C
    .word D
    .word R
