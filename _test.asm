; TC source file: test.tc
format PE console
include 'win32ax.inc'
;================== code =====================
.code
entry main
;================== library ==================
exit:	ret
;=============================================
T1:
	PUSH EAX
	MOV EAX, 11111
	MOV [_t1], EAX
	POP EAX
	PUSH EAX
	MOV EAX, 1
	MOV [t13], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, 17
	PUSH EAX
	MOV EAX, 8
	MOV EBX, EAX
	POP EAX
	IDIV EAX
	PUSH EAX
	MOV EAX, 2
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END45
	; THEN ...
	PUSH EAX
	MOV EAX, [t13]
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	MOV [t13], EAX
	POP EAX
.END45:
	PUSH EAX
	MOV EAX, 1
	MOV [t11], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, 2
	PUSH EAX
	MOV EAX, 3
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JLE @f
	INC DX
@@:	MOV EAX, EDX
	PUSH EAX
	MOV EAX, 3
	PUSH EAX
	MOV EAX, 4
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JE @f
	INC DX
@@:	MOV EAX, EDX
	POP EBX
	AND EAX, EBX
	PUSH EAX
	MOV EAX, 0
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END89
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t11], EAX
	POP EAX
.END89:
	PUSH EAX
	MOV EAX, 1
	MOV [t12], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, 2
	PUSH EAX
	MOV EAX, 3
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JLE @f
	INC DX
@@:	MOV EAX, EDX
	PUSH EAX
	MOV EAX, 3
	PUSH EAX
	MOV EAX, 4
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JE @f
	INC DX
@@:	MOV EAX, EDX
	POP EBX
	OR EAX, EBX
	PUSH EAX
	MOV EAX, 0
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END133
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t12], EAX
	POP EAX
.END133:
	RET

T2:
	push ebp
	mov ebp, esp
	CALL T1
	mov esp, ebp
	pop ebp
	PUSH EAX
	MOV EAX, 22222
	MOV [_t2], EAX
	POP EAX
	PUSH EAX
	MOV EAX, 1
	MOV [t2], EAX
	POP EAX
	PUSH EAX
	MOV EAX, 0
	MOV [c], EAX
	POP EAX
.WS154:
	PUSH EAX
	MOV EAX, [t2]
	PUSH EAX
	MOV EAX, 1025
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JGE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .WE185
	PUSH EAX
	MOV EAX, [c]
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	MOV [c], EAX
	POP EAX
	PUSH EAX
	MOV EAX, [t2]
	PUSH EAX
	MOV EAX, 2
	POP EBX
	IMUL EBX
	MOV [t2], EAX
	POP EAX
	JMP .WS154
.WE185: 
	PUSH EAX
	MOV EAX, 1
	MOV [t21], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, [t2]
	PUSH EAX
	MOV EAX, 2047
	PUSH EAX
	MOV EAX, [t21]
	POP EBX
	ADD EAX, EBX
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END213
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t21], EAX
	POP EAX
.END213:
	PUSH EAX
	MOV EAX, 1
	MOV [t22], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, [c]
	PUSH EAX
	MOV EAX, 11
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END237
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t22], EAX
	POP EAX
.END237:
	RET

T3:
	push ebp
	mov ebp, esp
	CALL T2
	mov esp, ebp
	pop ebp
	PUSH EAX
	MOV EAX, 33333
	MOV [_t3], EAX
	POP EAX
	PUSH EAX
	MOV EAX, 1
	MOV [t30], EAX
	POP EAX
	; IF ... ELSE ...
	PUSH EAX
	MOV EAX, 8
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	PUSH EAX
	MOV EAX, 12
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	POP EBX
	IMUL EBX
	PUSH EAX
	MOV EAX, 100
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .ELSE292
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t30], EAX
	POP EAX
	JMP .END298
.ELSE292:
	; ELSE ...
	PUSH EAX
	MOV EAX, 4
	MOV [t30], EAX
	POP EAX
.END298:
	RET

T4:
	push ebp
	mov ebp, esp
	CALL T3
	mov esp, ebp
	pop ebp
	PUSH EAX
	MOV EAX, 44444
	MOV [_t4], EAX
	POP EAX
	PUSH EAX
	MOV EAX, 0
	MOV [t40], EAX
	POP EAX
.WS315:
	PUSH EAX
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 10
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JGE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .WE338
	PUSH EAX
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	MOV [t40], EAX
	POP EAX
	JMP .WS315
.WE338: 
.DS339:
	PUSH EAX
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	MOV [t40], EAX
	POP EAX
	PUSH EAX
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 5
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JLE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JNZ .DS339
	; IF ...
	PUSH EAX
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 5
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END381
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t40], EAX
	POP EAX
.END381:
	PUSH EAX
	MOV EAX, 1
	MOV [t41], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 6
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JGE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END405
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t41], EAX
	POP EAX
.END405:
	RET

T41:
	RET

T5:
	push ebp
	mov ebp, esp
	CALL T4
	mov esp, ebp
	pop ebp
	push ebp
	mov ebp, esp
	CALL T41
	mov esp, ebp
	pop ebp
	PUSH EAX
	MOV EAX, 55555
	MOV [_t5], EAX
	POP EAX
	PUSH EAX
	MOV EAX, 1
	MOV [t50], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, 1
	PUSH EAX
	MOV EAX, 2
	POP EBX
	AND EAX, EBX
	PUSH EAX
	MOV EAX, 0
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END453
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t50], EAX
	POP EAX
.END453:
	PUSH EAX
	MOV EAX, 1
	MOV [t51], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, 1
	PUSH EAX
	MOV EAX, 2
	POP EBX
	OR EAX, EBX
	PUSH EAX
	MOV EAX, 3
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END481
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t51], EAX
	POP EAX
.END481:
	PUSH EAX
	MOV EAX, 1
	MOV [t52], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, 3
	PUSH EAX
	MOV EAX, 1
	POP EBX
	XOR EAX, EBX
	PUSH EAX
	MOV EAX, 2
	XOR EDX, EDX
	POP EBX
	CMP EAX, EBX
	JNE @f
	INC DX
@@:	MOV EAX, EDX
	TEST EAX, EAX
	POP EAX
	JZ .END509
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t52], EAX
	POP EAX
.END509:
	PUSH EAX
	MOV EAX, 1
	MOV [t53], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, [t53]
	TEST EAX, EAX
	POP EAX
	JZ .END525
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t53], EAX
	POP EAX
.END525:
	RET

main:
	push ebp
	mov ebp, esp
	CALL T5
	mov esp, ebp
	pop ebp
	PUSH EAX
	MOV EAX, 9678308
	MOV [ff], EAX
	POP EAX
	RET

;================== data =====================
.data
;=============================================
_t1:	dd 0
t13:	dd 0
t11:	dd 0
t12:	dd 0
_t2:	dd 0
t2:	dd 0
c:	dd 0
t21:	dd 0
t22:	dd 0
_t3:	dd 0
t30:	dd 0
_t4:	dd 0
t40:	dd 0
t41:	dd 0
_t5:	dd 0
t50:	dd 0
t51:	dd 0
t52:	dd 0
t53:	dd 0
ff:	dd 0

; symbols: 1000 entries, 28 used
; num type size val       name
; --- ---- ---- --------- -----------------
; 0   25   4    $0        exit
; 1   25   4    $1        T1
; 2   0    4    $0        _t1
; 3   0    4    $0        t13
; 4   0    4    $0        t11
; 5   0    4    $0        t12
; 6   25   4    $6        T2
; 7   0    4    $0        _t2
; 8   0    4    $0        t2
; 9   0    4    $0        c
; 10  0    4    $0        t21
; 11  0    4    $0        t22
; 12  25   4    $C        T3
; 13  0    4    $0        _t3
; 14  0    4    $0        t30
; 15  25   4    $F        T4
; 16  0    4    $0        _t4
; 17  0    4    $0        t40
; 18  0    4    $0        t41
; 19  25   4    $13       T41
; 20  25   4    $14       T5
; 21  0    4    $0        _t5
; 22  0    4    $0        t50
; 23  0    4    $0        t51
; 24  0    4    $0        t52
; 25  0    4    $0        t53
; 26  25   4    $1A       main
; 27  0    4    $0        ff
