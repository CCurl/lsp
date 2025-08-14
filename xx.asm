; **************************************************************************

format ELF executable

segment readable writable executable

; -------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------
entry _start

_start:
    mov eax,0x2d2d2d2d
    jmp main

exit:
    mov eax, 1      ; syscall number for exit
    mov ebx, 0      ; exit code 22
    int 0x80        ; call kernel

sPush:
    mov eax,0x77777777
    mov ecx, ebx
    mov ebx, eax
    ret

sPop:
    mov eax, ebx
    mov ebx, ecx
    ret
    mov eax,0x77777777

doEmit:
    mov eax, 4
    mov edx, 1
    int 0x80
doAdd: 
    call sPop
    add eax, ebx
    ret

main:
    mov eax,0x79797979
    lea esi, [stk]
    lea ecx, [xmsg]
    mov eax, 0x11223344
    call sPush
    mov eax, 0x11223344
    call sPop
    jmp exit

    mov eax,0x79797979

; segment readable writable
dtx:  db "-DATA-", 0
stk:  db 64 dup(0)
xmsg: db "hi there",0
here: dd 0
dte:  db 1024 dup(?)
