
    
    %ifndef BIN
      global _get_ctx
      global get_ctx
    %endif

    bits 32
    
get_ctx:
_get_ctx:
    push   edi        ; save edi if windoze
    
    push   ecx        ; put ptr to proc_ctx in edi
    pop    edi
    
    push   esp
    pop    eax
    shr    eax, 24
    jz     is_32
    pop    edi
    push   edi    
is_32:
    xor    eax, eax
    dec    eax
    neg    eax          ; ecx=0 if 64-bit
    jz     x64
    
    mov    edi, [esp+8] ; get proc_ctx from stack
x64:
    stosd
    lea    ecx, [eax-1]
    push   esp
    pop    eax
    shr    eax, 24
    setz   al
    stosd
    ; save segment registers
    mov    ax, cs
    stosw
    mov    ax, ds
    stosw
    mov    ax, es
    stosw
    mov    ax, fs
    stosw
    mov    ax, gs
    stosw
    mov    ax, ss
    stosw
    ;int3
    ; get stack pointer
    push   esp
    pop    eax
    stosd            ; save 32-bits of stack pointer
    jecxz  x32_native
    shr    eax, 32
    stosd            ; only if 64-bit
    push   ecx       ; save ecx since bsd trashes it
    push   edi       ; save edi because we need to use it
    
    push   -1        ; handle
    pop    edi
    push   6         ; 
    pop    eax       ; eax=sys_close
    syscall
    
    pop    edi       ; restore edi
    pop    ecx       ; restore ecx
    
    stosd
    shr    eax, 32
    stosd
x32_l3:
    pop    edi
    ret
x32_native:
    mov    cx, gs    ; win32 native? skip it
    jecxz  x32_l3
    
    push   esp
    pop    eax
    shr    eax, 24
    jz     x32_l3
    
    push   -1
    pop    ebx
    push   6
    pop    eax
    int    0x80
    stosd
    jmp    x32_l3
    
    
    
