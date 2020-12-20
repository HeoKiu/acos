.text
.global main



main:

    // r0, r1 = A, B
    push {r4, lr}
    pop {r4, lr}
    ldr r4, =var

    bl func
    ldr r0, [r4]
    ldr r0, [r4]
    ldr r0, [r4]
    ldr r0, [r4]
    mov lr, pc
    add lr, #4
    mov pc, r4
    ldr r0, [pc, #0]
    bx lr

func:
    bx lr

var:
    .word

