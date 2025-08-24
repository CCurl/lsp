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
	MOV 	EAX, 1
	MOV 	EBX, [y]
	MOV 	ECX, 10
	MOV 	EDX, [x]
	XCHG	EAX, ECX
	XCHG	ECX, EDX
	CDQ
	IDIV	ECX
	XCHG	EAX, ECX
	IMUL	EBX, ECX
	MOV 	ECX, 4
	SUB 	EBX, ECX
	SUB 	EAX, EBX
	MOV 	[x], EAX
	MOV 	EAX, [x]
	MOV 	EBX, 23
	XCHG	ECX, EBX
	CDQ
	IDIV	ECX
	MOV 	[y], EAX
	; IF #1 ...
	MOV 	EAX, [y]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	EBX, [x]
	MOV 	ECX, 2
	IMUL	EBX, ECX
	CMP 	EAX, EBX
	MOV 	EAX, 0
	JGE 	@F
	INC 	EAX
@@:
	TEST	EAX, EAX
	JNZ 	IF_01
	; Then #1 ...
	MOV 	EAX, [y]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	[y], EAX
	; ENDIF #1 ...
IF_01:
	MOV 	EAX, 1
	MOV 	[z], EAX
	; IF #2 ...
	MOV 	EAX, [y]
	MOV 	EBX, 5
	CMP 	EAX, EBX
	MOV 	EAX, 0
	JNE 	@F
	INC 	EAX
@@:
	TEST	EAX, EAX
	JNZ 	IF_02
	; Then #2 ...
	MOV 	EAX, [y]
	MOV 	EBX, 3
	IMUL	EAX, EBX
	MOV 	[y], EAX
	; ENDIF #2 ...
IF_02:
	MOV 	EAX, [z]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	[z], EAX
	; IF #3 ...
	MOV 	EAX, [y]
	MOV 	EBX, 5
	CMP 	EAX, EBX
	MOV 	EAX, 0
	JLE 	@F
	INC 	EAX
@@:
	TEST	EAX, EAX
	JNZ 	IF_03
	; Then #3 ...
	MOV 	EAX, [y]
	MOV 	EBX, 1
	SUB 	EAX, EBX
	MOV 	[y], EAX
	; ENDIF #3 ...
IF_03:
	RET
;---------------------------------------------
main:
	RET
;================== data =====================
.data
;=============================================

; symbols: 1000 entries, 8 used
; num type size name
; --- ---- ---- -----------------
_pc_buf:	db 0 DUP(4)
x:	db 0 DUP(4)
y:	db 0 DUP(4)
z:	db 0 DUP(4)
