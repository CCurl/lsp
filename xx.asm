; **************************************************************************

format ELF executable

segment readable writable

; nothing here

segment readable executable

; -------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------
; -------------------------------------------------------------------------------------
entry _start

_start:
    mov eax, 1      ; syscall number for exit
    xor ebx, ebx    ; exit code 0
    int 0x80        ; call kernel
