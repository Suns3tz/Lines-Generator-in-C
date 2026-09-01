section .data

section .bss

section .text
    global oct1
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
    ; if d>0 se colorea NE else se colorea E
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