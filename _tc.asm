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
	; store
	MOV [num], EAX
	POP EAX
	; fetch
	PUSH EAX
	MOV EAX, [num]
	; store
	MOV [x], EAX
	POP EAX
.WS30:
	; fetch
	PUSH EAX
	MOV EAX, [x]
	TEST EAX, EAX
	POP EAX
	JZ .WE49
	; fetch
	PUSH EAX
	MOV EAX, [x]
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	; store
	MOV [x], EAX
	POP EAX
	JMP .WS30
.WE49: 
	CALL exit
	RET

;================== data =====================
segment readable writeable
;=============================================
num:	dd 0
x:	dd 0
