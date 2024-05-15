global _start
_start:
    ; x
    mov rax, 5
    push rax
    ; y
    mov rax, 2
    push rax
    lea rsp, [rsp + 8]
    pop rax
    sub rsp, 8
    lea rsp, [rsp + 0]
    pop rdi
    sub rsp, 0
    add rax, rdi
    push rax
    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
