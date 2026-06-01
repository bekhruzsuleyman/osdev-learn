section .text
    global cpuid_call

cpuid_call:
    push rbx

    mov eax, edi
    mov ecx, esi
    cpuid

    mov [rdx], eax
    mov [rdx + 4], ebx
    mov [rdx + 8], ecx
    mov [rdx + 12], edx

    pop rbx
    ret