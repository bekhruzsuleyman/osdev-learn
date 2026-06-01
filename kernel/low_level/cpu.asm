section .text
    global read_cr0
    global write_cr0
    global read_cr3
    global write_cr3
    global read_cr4
    global write_cr4

read_cr0:
    mov rax, cr0
    ret

write_cr0:
    mov cr0, rdi
    ret

read_cr3:
    mov rax, cr3
    ret

write_cr3:
    mov cr3, rdi
    ret

read_cr4:
    mov rax, cr4
    ret

write_cr4:
    mov cr4, rdi
    ret