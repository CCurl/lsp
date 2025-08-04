; **************************************************************************

format ELF executable

segment readable writable executable

; -------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------
entry _start

_start:
    mov eax,0x2d2d2d2d

    mov eax, 1      ; syscall number for exit
    mov ebx, 22     ; exit code 22
    int 0x80        ; call kernel

    mov eax,0x79797979
    lea ecx, [xmsg]
    mov eax,0x7a7a7a7a

; segment readable writable
xaaa: db "-DATA-"
xmsg: db "hi there",0
xzzz: db "-DATAEND-"
