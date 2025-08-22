; TC source file: bm.tc
format ELF executable
;================== code =====================
segment readable executable
entry main
;================== library ==================
exit:
	MOV EAX, 1
	XOR EBX, EBX
	INT 0x80

putc:
	MOV [_pc_buf], EAX
	MOV EAX, 4
	MOV EBX, 0
	LEA ECX, [_pc_buf]
	MOV EDX, 1
	INT 0x80
	RET

;=============================================
TMil:
	PUSH EAX
	MOV EAX, [x]
	PUSH EAX
	MOV EAX, 1002
	POP EBX
	IMUL EBX
	PUSH EAX
	MOV EAX, 998
	POP EBX
	IMUL EBX
	POP EBX
	RET
	RET

Mil:
	PUSH EAX
	MOV EAX, [x]
	PUSH EAX
	MOV EAX, 1001
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	POP EBX
	IMUL EBX
	PUSH EAX
	MOV EAX, 999
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	POP EBX
	IMUL EBX
	MOV [x], EAX
	POP EAX
	RET

main:
	PUSH EAX
	MOV EAX, 500
	MOV [x], EAX
	POP EAX
	push ebp
	mov ebp, esp
	CALL Mil
	mov esp, ebp
	pop ebp
	PUSH EAX
	MOV EAX, [x]
	MOV [num], EAX
	POP EAX
.WS66:
	PUSH EAX
	MOV EAX, [x]
	TEST EAX, EAX
	POP EAX
	JZ .WE82
	PUSH EAX
	MOV EAX, [x]
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	MOV [x], EAX
	POP EAX
	JMP .WS66
.WE82: 
	push ebp
	mov ebp, esp
	CALL exit
	mov esp, ebp
	pop ebp
	RET

;================== data =====================
segment readable writeable
;=============================================
_pc_buf:	dd 0
num:	dd 0
x:	dd 0

; symbols: 1000 entries, 7 used
; num type size val       name
; --- ---- ---- --------- -----------------
; 0   25   4    $0        exit
; 1   0    4    $0        _pc_buf
; 2   0    4    $0        num
; 3   0    4    $0        x
; 4   25   4    $0        TMil
; 5   25   4    $0        Mil
; 6   25   4    $0        main
