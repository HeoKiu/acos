#! /bin/bash

arm-linux-gnueabi-gcc -marm main.S -o main
echo -aab12345fjdsofjsodjfodfjsfsadiofj11-| qemu-arm -L /home/boris/sysroot-glibc-linaro-2.25-2018.05-arm-linux-gnueabi main

