.code

_call_stub proc
    pop r11             ; r11 = return address
    add rsp, 8          ; remove reserved space
    mov rax, [rsp + 24] ; rax = shell param
    
    mov r10, [rax]      ; r10 = trampoline address
    mov [rsp], r10      ; set trampoline address as return address

    mov r10, [rax + 8]  ; r10 = function address
    mov [rax + 8], r11  ; store original return address in the shell param

    mov [rax + 16], rbx ; store rbx in shell_param.rbx
    lea rbx, fixup
    mov [rax], rbx      ; 
    mov rbx, rax

    jmp r10

fixup:
    sub rsp, 16
    mov rcx, rbx
    mov rbx, [rcx + 16]
    jmp QWORD PTR [rcx + 8]

_call_stub endp

end