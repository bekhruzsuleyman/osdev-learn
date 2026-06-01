section .text
    global cpuid_call

cpuid_call:
    push rbx

    mov eax, edi
    mov ecx, esi
    mov r8, rdx
    cpuid

    mov [r8], eax
    mov [r8 + 4], ebx
    mov [r8 + 8], ecx
    mov [r8 + 12], edx

    pop rbx
    ret