; TC source file: bm.tc
format ELF executable
;================== code =====================
segment readable executable
;=============================================
entry main
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

main:
	PUSH EAX
	MOV EAX, 1000
	PUSH EAX
	MOV EAX, 1000
	POP EBX
	IMUL EBX
	PUSH EAX
	MOV EAX, 1000
	POP EBX
	IMUL EBX
	MOV [num], EAX
	POP EAX
	PUSH EAX
	MOV EAX, [num]
	MOV [x], EAX
	POP EAX
.WS30:
	PUSH EAX
	MOV EAX, [x]
	TEST EAX, EAX
	POP EAX
	JZ .WE46
	PUSH EAX
	MOV EAX, [x]
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	MOV [x], EAX
	POP EAX
	JMP .WS30
.WE46: 
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
