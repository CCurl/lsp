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

sPushEAX:
    lea esi, [esi+4]
    mov [esi], eax
    mov eax, ebx
    ret

sPop:
    xchg ebx, eax
    mov eax, [esi]
    lea esi, [esi-4]
    ret

doEmit:
    mov eax, 4
    mov edx, 1
    int 0x80

main:
    mov eax,0x79797979
    lea esi, [stk]
    lea ecx, [xmsg]
    mov eax,0x11223344
    call sPushEAX
    call sPop

doAdd: 
    call sPop
    add ebx
    ret

main:
    mov eax,0x79797979
    lea esi, [stk]
    lea ecx, [xmsg]
    mov eax, 0x11223344
    call sPushEAX
    call sPop
    jmp exit

    mov eax,0x79797979

; segment readable writable
dt:   db "-DATA-", 0
stk:  db 64 dup(0)
msg:  db "hi there",0
here: dd 0
dte:  db 1024*1024 dup(?)
