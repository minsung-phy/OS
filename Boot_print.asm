[org 0]
[bits 16]

jmp 0x07C0:start

start:

mov ax, 0xB800
mov es, ax

mov byte[es:0], 'h'
mov byte[es:1], 0x09
mov byte[es:2], 'i'
mov byte[es:3], 0x09

jmp $

times 510-($-$$) db 0
dw 0xAA55