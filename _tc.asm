format PE console
include 'win32ax.inc'
.code
entry main
; source: bm.tc
main:
	mov ebx, 0x2a2a2a2a
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
	; POP EAX
	mov ebx, 0x2a2a2a2a
.WS25:
	; fetch
	;PUSH EAX
	;MOV EAX, [x]
	TEST EAX, EAX
	;POP EAX
	JZ .WE44
	DEC EAX
	; fetch
	;PUSH EAX
	;MOV EAX, [x]
	;PUSH EAX
	;MOV EAX, 1
	;MOV EBX, EAX
	;POP EAX
	;SUB EAX, EBX
	; store
	;MOV [x], EAX
	;POP EAX
	JMP .WS25
.WE44: 
	RET
	mov ebx, 0x2a2a2a2a

.data
num:	dd 0
x:	dd 0
