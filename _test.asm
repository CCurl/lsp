; TC source file: test.tc
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
	JZ .END50
	; THEN ...
	PUSH EAX
	MOV EAX, [t13]
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	MOV [t13], EAX
	POP EAX
.END50:
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
	JZ .END94
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t11], EAX
	POP EAX
.END94:
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
	JZ .END138
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t12], EAX
	POP EAX
.END138:
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
.WS159:
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
	JZ .WE190
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
	JMP .WS159
.WE190: 
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
	JZ .END218
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t21], EAX
	POP EAX
.END218:
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
	JZ .END242
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t22], EAX
	POP EAX
.END242:
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
	JZ .ELSE297
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t30], EAX
	POP EAX
	JMP .END303
.ELSE297:
	; ELSE ...
	PUSH EAX
	MOV EAX, 4
	MOV [t30], EAX
	POP EAX
.END303:
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
.WS320:
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
	JZ .WE343
	PUSH EAX
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	MOV [t40], EAX
	POP EAX
	JMP .WS320
.WE343: 
.DS344:
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
	JNZ .DS344
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
	JZ .END386
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t40], EAX
	POP EAX
.END386:
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
	JZ .END410
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t41], EAX
	POP EAX
.END410:
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
	JZ .END458
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t50], EAX
	POP EAX
.END458:
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
	JZ .END486
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t51], EAX
	POP EAX
.END486:
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
	JZ .END514
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t52], EAX
	POP EAX
.END514:
	PUSH EAX
	MOV EAX, 1
	MOV [t53], EAX
	POP EAX
	; IF ...
	PUSH EAX
	MOV EAX, [t53]
	TEST EAX, EAX
	POP EAX
	JZ .END530
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t53], EAX
	POP EAX
.END530:
	RET

main:
	push ebp
	mov ebp, esp
	CALL T5
	mov esp, ebp
	pop ebp
	PUSH EAX
	LEA EAX, [_s001_]
	MOV [ff], EAX
	POP EAX
	RET

;================== data =====================
segment readable writeable
;=============================================
_pc_buf:	dd 0
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
_s001_:	db "hi there", 0

; symbols: 1000 entries, 30 used
; num type size val       name
; --- ---- ---- --------- -----------------
; 0   25   4    $0        exit
; 1   0    4    $0        _pc_buf
; 2   25   4    $0        T1
; 3   0    4    $0        _t1
; 4   0    4    $0        t13
; 5   0    4    $0        t11
; 6   0    4    $0        t12
; 7   25   4    $0        T2
; 8   0    4    $0        _t2
; 9   0    4    $0        t2
; 10  0    4    $0        c
; 11  0    4    $0        t21
; 12  0    4    $0        t22
; 13  25   4    $0        T3
; 14  0    4    $0        _t3
; 15  0    4    $0        t30
; 16  25   4    $0        T4
; 17  0    4    $0        _t4
; 18  0    4    $0        t40
; 19  0    4    $0        t41
; 20  25   4    $0        T41
; 21  25   4    $0        T5
; 22  0    4    $0        _t5
; 23  0    4    $0        t50
; 24  0    4    $0        t51
; 25  0    4    $0        t52
; 26  0    4    $0        t53
; 27  25   4    $0        main
; 28  0    4    $0        ff
; 29  2    4    $565F329C _s001_
