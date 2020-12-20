.text
.global main
.extern printf

main:
    push {lr}
    ldr r0, =scanf_format_str
    ldr r1, =var1
    ldr r2, =var2
    bl scanf
    ldr r1, =var1
    ldr r1, [r1]
    ldr r2, =var2
    ldr r2, [r2]

    add r1, r1, r2

    ldr r0, =printf_format_str
    bl printf

    pop {lr}
    bx lr

.data
var1:
    .space 4
var2:
    .space 4
scanf_format_str:
    .asciz "%d%d"

printf_format_str:
    .asciz "%d\n"
