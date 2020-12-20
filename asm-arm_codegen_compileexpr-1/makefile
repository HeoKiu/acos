compile_test_c:
	arm-linux-gnueabi-gcc test.c -c -o test.o

compile_jit: compile_test_c
	arm-linux-gnueabi-g++ jit_compile_arm.cpp test.o -o a.out

run: compile_jit
	cat input.txt | qemu-arm -L /home/boris/sysroot-glibc-linaro-2.25-2018.05-arm-linux-gnueabi a.out