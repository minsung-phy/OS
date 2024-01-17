CC = gcc

final.img : Boot_c.img Sector2_c.img disk.img
	cat Boot_c.img Sector2_c.img disk.img > final.img

disk.img : main.img
	objcopy -O binary main.img disk.img

main.img : main.o function.o
	ld -melf_i386 -Ttext 0x10200 -nostdlib main.o function.o -o main.img

main.o : main.c
	gcc -c -m32 -ffreestanding main.c -o main.o

function.o : function.c
	gcc -c -m32 -ffreestanding function.c -o function.o

Boot_c.img : Boot_c.asm
	nasm -f bin -o Boot_c.img Boot_c.asm

Sector2_c.img : Sector2_c.asm
	nasm -f bin -o Sector2_c.img Sector2_c.asm

clean :
	rm *.o
