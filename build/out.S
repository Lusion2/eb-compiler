global _start
_start:
    ; x
    mov rax, 2
    mov rdi, 3
    add rax, rdi
    push rax
    ; y
    push QWORD [rsp + -8]
    pop rax
    mov rax, 60
    push QWORD [rsp + -8]
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
