
    
    %ifndef BIN
      global _get_ctx
      global get_ctx
    %endif

    bits 32
    
get_ctx:
_get_ctx:
    push   edi          ; save edi for windows/bsd/linux
    push   ebx          ; save ebx for 32-bit bsd/linux
    
    push   ecx          ; for windows, put ptr to proc_ctx in edi
    pop    edi
    
    push   esp
    pop    eax
    shr    eax, 24
    jz     is_32        ; we're windows
    
    pop    edi          ; we're linux/bsd/osx
    push   edi    
is_32:
    xor    eax, eax     ; eax=0
    dec    eax          ; ignored if 64-bit
    neg    eax          ; if eax==0 goto x64
    jz     x64
    
    mov    edi, [esp+12] ; get proc_ctx from stack
x64:
    stosd               ; save emu value
    lea    ecx, [eax-1]
    push   esp
    pop    eax
    shr    eax, 24
    setz   al
    stosd               ; save win value
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
    stosd
    jecxz  x32_native
    dec    eax
    ror    eax, 32
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
    dec    eax
    ror    eax, 32
    stosd
x32_l3:
    pop    ebx
    pop    edi
    push   1
    pop    eax
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
    
    
    
