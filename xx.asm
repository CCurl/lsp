; **************************************************************************

format ELF executable

segment readable executable

; -------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------
entry _start

_start:
    mov eax,0x41414141

    mov eax, 1      ; syscall number for exit
    mov ebx, 22     ; exit code 22
    int 0x80        ; call kernel

    mov eax,0x42424242
    lea ecx, [xmsg]
    mov eax,0x5A5A5A5A

segment readable writable
xaaa: db "aaaaaaaa",0
xmsg: db "hi there",0
xzzz: db "zzzzzzzz",0
