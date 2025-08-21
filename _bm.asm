; TC source file: bm.tc
format PE console
include 'win32ax.inc'
;================== code =====================
.code
entry main
;================== library ==================
exit:	ret
;=============================================
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
.WS25:
	PUSH EAX
	MOV EAX, [x]
	TEST EAX, EAX
	POP EAX
	JZ .WE41
	PUSH EAX
	MOV EAX, [x]
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	MOV [x], EAX
	POP EAX
	JMP .WS25
.WE41: 
	push ebp
	mov ebp, esp
	CALL exit
	mov esp, ebp
	pop ebp
	RET

;================== data =====================
.data
;=============================================
num:	dd 0
x:	dd 0

; symbols: 1000 entries, 4 used
; num type size val       name
; --- ---- ---- --------- -----------------
; 0   25   4    $0        exit
; 1   0    4    $0        num
; 2   0    4    $0        x
; 3   25   4    $3        main
