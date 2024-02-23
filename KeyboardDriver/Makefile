# $@ = target file
# $< = first dependency
# $^ = all dependencies

# Detect all .o files based on their .c source
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c)
HEADERS = $(wildcard kernel/*.h  drivers/*.h cpu/*.h)
OBJ_FILES = $(patsubst %.c, %.o, $(C_SOURCES)) cpu/interrupt.o

# First rule is the one executed when no parameters are fed to the Makefile
all: run

# Notice how dependencies are built as needed
kernel.bin: boot/kernel_entry.o $(OBJ_FILES)
	gcc -m32 -o kernel.elf -Ttext 0x1000 $^ -nostdlib -lgcc_s
	objcopy -O binary kernel.elf $@

os-image.bin: boot/mbr.bin kernel.bin
	cat $^ > $@

run: os-image.bin
	qemu-system-i386 -fda $<

echo: os-image.bin
	xxd $<

# Only for debug
kernel.elf: boot/kernel_entry.o $(OBJ_FILES)
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -fda os-image.bin -d guest_errors,int &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c $(HEADERS)
	gcc -g -m32 -ffreestanding -fno-pic -c $< -o $@ # -g for debugging, added -fno-pic

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

%.dis: %.bin
	ndisasm -b 32 $< > $@

clean:
	rm -rf *.bin *.o *.dis *.elf
	rm -rf kernel/*.o
	rm -rf boot/*.o boot/*.bin
	rm -rf drivers/*.o
	rm -rf cpu/*.o

