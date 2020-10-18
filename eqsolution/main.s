    .global solve
    .text

solve:
    // r0 - A, r1 - B, r2 - C, r3 - D
    // r4 - x, r5 - x^n, r6 - answer = ax^3 + bx^2 + cx + d, r7 - help
    push {r4, r5, r6, r7}
    mov r4, #0 // int x = 0

whilebegin:

    mov r6, r3 // answer = D

    mov r5, r4 // x^1 = x
    mul r7, r5, r2 // r7 = x^1 * C
    add r6, r6, r7 // answer += r7

    mul r7, r5, r4 // r7 = x^1 * x
    mov r5, r7 // r5 = r7
    mul r7, r5, r1 // r7 = x^2 * b
    add r6, r6, r7 // answer += r7

    mul r7, r5, r4 // r5 *= x
    mov r5, r7 // r5 = r7
    mul r7, r5, r0 // r7 = x^3 * A
    add r6, r6, r7 // answer += r7

    add r4, r4, #1 // x += 1
    cmp r6, #0 // answer <> 0
    bne whilebegin // if answer != 0 -> goto whilebegin

    mov r0, r4 // r0 = x
    sub r0, r0, #1 //r0 -= 1

    pop {r4, r5, r6, r7}
    bx lr
