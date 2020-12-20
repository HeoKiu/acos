disassembly_main:
	arm-linux-gnueabi-gcc -S main.c -o main.S

compile_main:
	arm-linux-gnueabi-gcc main.c -o main.o

show_main_o: compile_main
	arm-linux-gnueabi-objdump -d main.o


compile_asm:
	arm-linux-gnueabi-as -c asm.s -o asm.o

build_asm:
	arm-linux-gnueabi-as asm.s -o asm.out

show_asm: compile_asm
	arm-linux-gnueabi-objdump -d asm.o