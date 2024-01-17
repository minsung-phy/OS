[org 0]
[bits 16]

jmp 0x07C0:start

start:
mov ax, cs
mov ds, ax
mov es, ax

mov ax, 0xB800
mov es, ax

mov byte[es:0], 'h'
mov byte[es:1], 0x09
mov byte[es:2], 'i'
mov byte[es:3], 0x09

read:
	mov ax, 0x1000
	mov es, ax
	mov bx, 0

	mov ah, 2 
	mov al, 2
	mov ch, 0 
	mov cl, 2 
	mov dh, 0 
	mov dl, 0 
	int 13h
	
	jc read 

	mov dx, 0x3F2 
	xor al, al
	out dx, al 
	
	cli

lgdt[gdtr]
	
mov eax, cr0
or eax, 1
mov cr0, eax
	
jmp $+2
nop
nop

mov bx, DataSegment
mov ds, bx
mov es, bx
mov fs, bx
mov gs, bx
mov ss, bx

jmp dword CodeSegment:0x10000


gdtr:
dw gdt_end - gdt - 1
dd gdt+0x7C00

gdt:

	dd 0,0 
	CodeSegment equ 0x08
	dd 0x0000FFFF, 0x00CF9A00 
	DataSegment equ 0x10
	dd 0x0000FFFF, 0x00CF9200 
	VideoSegment equ 0x18
	dd 0x8000FFFF, 0x0040920B

gdt_end:

times 510-($-$$) db 0
dw 0xAA55

