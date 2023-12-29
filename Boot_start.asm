[org 0]
[bits 16]

jmp 0x07C0:start

start:


jmp $

times 510-($-$$) db 0
dw 0xAA55