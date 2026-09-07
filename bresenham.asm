section .data

section .bss

section .text
    global oct1
    global oct8
    extern plot

; vars en C vienen así:
; x0 en edi
; y0 en esi
; x1 en edx
; y1 en ecx

; delta_e va a estar en  ebx
; delta_ne va a estar en ebp
; xp va a estar en r12d
; yp va a estar en r13d
; d va a estar en r14d
; x1 va a estar en r15d

; - - - - - OCTANTE 1

oct1:

    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_e = 2*(y1-y0)
    mov eax, ecx
    sub eax, esi

    shl eax, 1
    mov ebx, eax

    ; delta_ne = 2*((y1-y0)-(x1-x0))
    mov eax, ecx
    sub eax, esi

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(y1-y0) - (x1-x0)
    mov eax, ecx
    sub eax, esi

    shl eax, 1

    mov esi, edx
    sub esi, edi

    sub eax, esi
    mov r14d, eax

    ; Guardar x1
    mov r15d, edx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

; entrar en el while
_colorLoop:
    cmp r12d, r15d
    ; while xp < x1 siga, else termina
    jge _final

    cmp r14d, 0
    ; if d > 0 se colorea NE else se colorea E
    jle _colorE
    jmp _colorNE

_colorE:
    ; pintar E, xp++, d = d+delta_e
    add r12d, 1
    add r14d, ebx

    jmp _plot

_colorNE:
    ; pintar NE, xp++, yp++, d = d+delta_ne
    add r12d, 1
    add r13d, 1
    add r14d, ebp

    jmp _plot

_plot:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop

_final:
    add rsp, 8

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    ret

; - - - - - OCTANTE 5

oct5:

    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_S = 2*(y1-y0)
    mov eax, ecx
    sub eax, esi

    shl eax, 1
    mov ebx, eax

    ; delta_SO = 2*((y1-y0)-(x1-x0))
    mov eax, ecx
    sub eax, esi

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(y1-y0) - (x1-x0)
    mov eax, ecx
    sub eax, esi

    shl eax, 1

    mov esi, edx
    sub esi, edi

    sub eax, esi
    mov r14d, eax

    ; Guardar x1
    mov r15d, edx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

    ; entrar en el while
_colorLoop:
    cmp r12d, r15d
    ; while xp > x1 siga, else termina
    jge _final

    cmp r14d, 0
    ; if d>=0 se colorea SO else se colorea O
    jle _colorO
    jmp _colorSO

_colorO:
    ; pintar O, xp--, d = d+delta_O
    sub r12d, 1
    add r14d, ebx

    jmp _plot

_colorSO:
    ; pintar SO, xp--, yp--, d = d+delta_SO
    sub r12d, 1
    sub r13d, 1
    add r14d, ebp

    jmp _plot

_plot:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop

_final:
    add rsp, 8

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    ret
    
; - - - - - OCTANTE 6

oct6:

    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_S = 2*(x0-x1)
    mov eax, edi
    sub eax, edx

    shl eax, 1
    mov ebx, eax

    ; delta_SO = 2*((x0-x1)-(y0-y1))
    mov eax, edi
    sub eax, edx

    mov r8d, esi
    sub r8d, ecx

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(x0-x1) - (y0-y1)
    mov eax, edi
    sub eax, edx

    shl eax, 1

    sub eax, r8d
    mov r14d, eax

    ; Guardar y1
    mov r15d, ecx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

    ; entrar en el while
_colorLoop:
    cmp r13d, r15d
    ; while yp > y1 siga, else termina
    jle _final

    cmp r14d, 0
    ; if d>=0 se colorea SO else se colorea S
    jge _colorSO
    jmp _colorS

_colorS:
    ; pintar S, yp--, d = d+delta_S
    sub r13d, 1
    add r14d, ebx

    jmp _plot

_colorSO:
    ; pintar SO, xp--, yp--, d = d+delta_SO
    sub r12d, 1
    sub r13d, 1
    add r14d, ebp

    jmp _plot

_plot:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop

_final:
    add rsp, 8

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    ret



; - - - - - OCTANTE 7

oct7:

    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_S = 2*(x1-x0)
    mov eax, edx
    sub eax, edi

    shl eax, 1
    mov ebx, eax

    ; delta_SE = 2*((x1-x0)-(y0-y1))
    mov eax, edx
    sub eax, edi

    mov r8d, esi
    sub r8d, ecx

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(x1-x0) - (y0-y1)
    mov eax, edx
    sub eax, edi

    shl eax, 1
    
    mov r8d, esi
    sub r8d, ecx

    sub eax, r8d
    mov r14d, eax

    ; Guardar y1
    mov r15d, ecx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

    ; entrar en el while
_colorLoop:
    cmp r13d, r15d
    ; while yp > y1 siga, else termina
    jle _final

    cmp r14d, 0
    ; if d>=0 se colorea SE else se colorea S
    jge _colorSE
    jmp _colorS

_colorS:
    ; pintar S, yp--, d = d+delta_S
    sub r13d, 1
    add r14d, ebx

    jmp _plot

_colorSE:
    ; pintar SO, xp++, yp--, d = d+delta_SO
    add r12d, 1
    sub r13d, 1
    add r14d, ebp

    jmp _plot

_plot:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop

_final:
    add rsp, 8

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    ret



; - - - - - OCTANTE 8
oct8:

    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_e = 2*(y0-y1)
    mov eax, esi
    sub eax, ecx

    shl eax, 1
    mov ebx, eax


    ; delta_se = 2*((y0-y1)-(x1-x0))
    mov eax, esi
    sub eax, ecx

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    shl eax, 1
    mov ebp, eax


    ; xp = x0, yp = y0

    mov r12d, edi
    mov r13d, esi


    ; d = 2*(y0-y1) - (x1-x0)
    mov eax, esi
    sub eax, ecx

    shl eax, 1

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    mov r14d, eax

    mov r15d, edx


    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

; while (xp < x1)
_colorLoop8:
    cmp r12d, r15d
    jge _final8

    ; if (d <= 0) ir E
    cmp r14d, 0
    jle _colorE8

    ; else ir SE
    jmp _colorSE8

; pintar E, xp++, d = d + delta_e
_colorE8:
    add r12d, 1
    add r14d, ebx

    jmp _plot8

; pintar SE, xp++, yp--, d = d + delta_se
_colorSE8:
    add r12d, 1
    sub r13d, 1

    add r14d, ebp
    jmp _plot8

; plot(xp, yp)
_plot8:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop8

_final8:
    add rsp, 8

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    ret

section .note.GNU-stack noalloc noexec nowrite progbits
