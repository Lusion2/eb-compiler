global _start
_start:
    ; x
    mov rax, 7
    push rax
    ; y
    mov rax, 2
    push rax
    mov rax, 60
    push QWORD [rsp + 4]
    pop rdi
    sub rsp, 4
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
