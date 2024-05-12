global _start
_start:
    ; x
    mov rax, 213
    push rax
    ; y
    mov rax, 23
    push rax
    ; z
    mov rax, 1
    mov rdi, 2
    add rax, rdi
    push rax
    mov rax, 60
    lea rsp, [rsp + 16]
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
