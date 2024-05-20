global _start
_start:
    ; x
    mov rax, 1
    push rax
    mov rax, 2
    mov rdi, 3
    add rax, rdi
    push rax
    pop rax
    add rax, rdi
    push rax
    ; y
    mov rax, 2
    mov rdi, 2
    add rax, rdi
    push rax
    ; z
    push QWORD [rsp + 16]
    pop rax
    push QWORD [rsp + 0]
    pop rdi
    add rax, rdi
    push rax
    mov rax, 60
    push QWORD [rsp + 24]
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
