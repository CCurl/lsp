format ELF executable
;================== code =====================
segment readable executable
entry main
;================== library ==================
init:
	LEA EDI, [stk]
	ADD EDI, 1024
	RET


exit:
	MOV EAX, 1
	XOR EBX, EBX
	INT 0x80

puts:
	MOV [pv], EAX
	RET
putd:
	MOV [pv], EAX
	RET
putc:
	MOV [pv], EAX
	MOV EAX, 4
	MOV EBX, 0
	LEA ECX, [pv]
	MOV EDX, 1
	INT 0x80
	RET

;=============================================
T4:
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL x
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL num
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	DEC []
	CMP_GT EAX, EAX
	XCHG ESP, EDI
	CALL num
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	INC []
	XCHG ESP, EDI
	CALL x
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	RET
Mil:
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	POP EBX
	POP EAX
	IMUL EAX, EBX
	PUSH EAX
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	POP EBX
	POP EAX
	IMUL EAX, EBX
	PUSH EAX
	XCHG ESP, EDI
	RET
main:
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL Mil
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL x
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL x
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	DEC []
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	POP EAX
	SUB [ESP], EAX
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
	XCHG ESP, EDI
	RET
	XCHG ESP, EDI
	CALL 
	XCHG ESP, EDI
;================== data =====================
segment readable writeable
;=============================================

; symbols: 100 entries, 9 used
; num type size name
; --- ---- ---- -----------------
_v_2       dd 0 ; pv
_v_3       dd 0 ; num
_v_4       dd 0 ; x
_v_6       dd 0 ; x
_v_7       dd 0 ; y
stk        rd 256
