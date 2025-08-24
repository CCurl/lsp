; TC source file: test.tc
format PE console
include 'win32ax.inc'
;================== code =====================
.code
entry main
;================== library ==================
exit:	RET

putc:	RET

;=============================================
;---------------------------------------------
T1:
	MOV 	EAX, [x]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	EBX, [y]
	MOV 	ECX, 10
	MOV 	EDX, [x]
	XCHG	EAX, ECX
	XCHG	ECX, EDX
	CDQ
	IDIV	ECX
	XCHG	EAX, ECX
	IMUL	EBX, ECX
	MOV 	ECX, 44
	ADD 	EBX, ECX
	SUB 	EAX, EBX
	MOV 	EAX, [y]
	MOV 	EBX, 23
	XCHG	ECX, EBX
	CDQ
	IDIV	ECX
	RET
;---------------------------------------------
main:
	RET
;================== data =====================
.data
;=============================================

; symbols: 1000 entries, 7 used
; num type size name
; --- ---- ---- -----------------
_pc_buf:	db 0 DUP(4)
x:	db 0 DUP(4)
y:	db 0 DUP(4)
