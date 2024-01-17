CC = gcc

final.img : Boot_c.img Sector2_c.img disk.img
	cat Boot_c.img Sector2_c.img disk.img > final.img

disk.img : HelloWorld.img
	objcopy -O binary HelloWorld.img disk.img

HelloWorld.img :HelloWorld.o
	ld -melf_i386 -Ttext 0x10200 -nostdlib HelloWorld.o -o HelloWorld.img

HelloWorld.o : HelloWorld.c
	gcc -c -m32 -ffreestanding HelloWorld.c -o HelloWorld.o

Boot_c.img : Boot_c.asm
	nasm -f bin -o Boot_c.img Boot_c.asm

Sector2_c.img : Sector2_c.asm
	nasm -f bin -o Sector2_c.img Sector2_c.asm

clean :
	rm *.o