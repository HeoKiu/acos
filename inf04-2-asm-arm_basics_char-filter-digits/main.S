.text
.global main
.extern fgetc, fputc, stdin, stdout


main:
    push {lr}

getsymbol:
    // r0 = get_one_symbol_from_stdin()
    ldr r0, =stdin
    ldr r0, [r0]
    bl fgetc

    // if(symbol == EOF) return 0;
    cmp r0, #-1
    bleq exit_main

    // '0' == 48 and '9' == 58
    // if(symbol < 48) goto getsymbol
    cmp r0, #48
    blt getsymbol
    // if(symbol > 58) goto getsymbol
    cmp r0, #58
    bgt getsymbol

    ldr r1, =stdout
    ldr r1, [r1]
    bl fputc

    // goto getsymbol
    b getsymbol

exit_main:
    pop {lr}
    bx lr


.data
    var:
        .space 4