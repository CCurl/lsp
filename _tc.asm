format PE console
include 'win32ax.inc'
.code
entry main
; source: test.tc
T1:
	PUSH EAX
	MOV EAX, 11111
	MOV [_t1], EAX ; store
	POP EAX ; store
	PUSH EAX
	MOV EAX, 1
	MOV [t13], EAX ; store
	POP EAX ; store
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
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END37
	; THEN ...
	MOV EAX, [t13]
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	MOV [t13], EAX ; store
	POP EAX ; store
.END37:
	PUSH EAX
	MOV EAX, 1
	MOV [t11], EAX ; store
	POP EAX ; store
	; IF ...
	PUSH EAX
	MOV EAX, 2
	PUSH EAX
	MOV EAX, 3
	; greaterthan
	MOV EAX, EBX
	PUSH EAX
	MOV EAX, 3
	PUSH EAX
	MOV EAX, 4
	; not equals
	MOV EAX, EBX
	POP EBX
	AND EAX, EBX
	PUSH EAX
	MOV EAX, 0
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END69
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t11], EAX ; store
	POP EAX ; store
.END69:
	PUSH EAX
	MOV EAX, 1
	MOV [t12], EAX ; store
	POP EAX ; store
	; IF ...
	PUSH EAX
	MOV EAX, 2
	PUSH EAX
	MOV EAX, 3
	; greaterthan
	MOV EAX, EBX
	PUSH EAX
	MOV EAX, 3
	PUSH EAX
	MOV EAX, 4
	; not equals
	MOV EAX, EBX
	POP EBX
	OR EAX, EBX
	PUSH EAX
	MOV EAX, 0
	; not equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END101
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t12], EAX ; store
	POP EAX ; store
.END101:
	RET

T2:
	CALL T1
	PUSH EAX
	MOV EAX, 22222
	MOV [_t2], EAX ; store
	POP EAX ; store
	PUSH EAX
	MOV EAX, 1
	MOV [t2], EAX ; store
	POP EAX ; store
	PUSH EAX
	MOV EAX, 0
	MOV [c], EAX ; store
	POP EAX ; store
.WS118:
	MOV EAX, [t2]
	PUSH EAX
	MOV EAX, 1025
	; less than
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .WE142
	MOV EAX, [c]
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	MOV [c], EAX ; store
	POP EAX ; store
	MOV EAX, [t2]
	PUSH EAX
	MOV EAX, 2
	POP EBX
	IMUL EAX
	MOV [t2], EAX ; store
	POP EAX ; store
	JMP .WS118
.WE142: 
	PUSH EAX
	MOV EAX, 1
	MOV [t21], EAX ; store
	POP EAX ; store
	; IF ...
	MOV EAX, [t2]
	PUSH EAX
	MOV EAX, 2047
	MOV EAX, [t21]
	POP EBX
	ADD EAX, EBX
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END164
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t21], EAX ; store
	POP EAX ; store
.END164:
	PUSH EAX
	MOV EAX, 1
	MOV [t22], EAX ; store
	POP EAX ; store
	; IF ...
	MOV EAX, [c]
	PUSH EAX
	MOV EAX, 11
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END183
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t22], EAX ; store
	POP EAX ; store
.END183:
	RET

T3:
	CALL T2
	PUSH EAX
	MOV EAX, 33333
	MOV [_t3], EAX ; store
	POP EAX ; store
	PUSH EAX
	MOV EAX, 1
	MOV [t30], EAX ; store
	POP EAX ; store
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
	IMUL EAX
	PUSH EAX
	MOV EAX, 100
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .ELSE230
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t30], EAX ; store
	POP EAX ; store
	JMP .END236
.ELSE230:
	; ELSE ...
	PUSH EAX
	MOV EAX, 4
	MOV [t30], EAX ; store
	POP EAX ; store
.END236:
	RET

T4:
	CALL T3
	PUSH EAX
	MOV EAX, 44444
	MOV [_t4], EAX ; store
	POP EAX ; store
	PUSH EAX
	MOV EAX, 0
	MOV [t40], EAX ; store
	POP EAX ; store
.WS249:
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 10
	; less than
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .WE266
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 1
	POP EBX
	ADD EAX, EBX
	MOV [t40], EAX ; store
	POP EAX ; store
	JMP .WS249
.WE266: 
.DS267:
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	SUB EAX, EBX
	MOV [t40], EAX ; store
	POP EAX ; store
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 5
	; greaterthan
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JNZ .DS267
	; IF ...
	MOV EAX, [t40]
	PUSH EAX
	MOV EAX, 5
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END298
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t40], EAX ; store
	POP EAX ; store
.END298:
	RET

T5:
	CALL T4
	PUSH EAX
	MOV EAX, 55555
	MOV [_t5], EAX ; store
	POP EAX ; store
	PUSH EAX
	MOV EAX, 1
	MOV [t50], EAX ; store
	POP EAX ; store
	; IF ...
	PUSH EAX
	MOV EAX, 1
	PUSH EAX
	MOV EAX, 2
	POP EBX
	AND EAX, EBX
	PUSH EAX
	MOV EAX, 0
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END330
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t50], EAX ; store
	POP EAX ; store
.END330:
	PUSH EAX
	MOV EAX, 1
	MOV [t51], EAX ; store
	POP EAX ; store
	; IF ...
	PUSH EAX
	MOV EAX, 1
	PUSH EAX
	MOV EAX, 2
	POP EBX
	OR EAX, EBX
	PUSH EAX
	MOV EAX, 3
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END354
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t51], EAX ; store
	POP EAX ; store
.END354:
	PUSH EAX
	MOV EAX, 1
	MOV [t52], EAX ; store
	POP EAX ; store
	; IF ...
	PUSH EAX
	MOV EAX, 3
	PUSH EAX
	MOV EAX, 1
	POP EBX
	XOR EAX, EBX
	PUSH EAX
	MOV EAX, 2
	; equals
	MOV EAX, EBX
	TEST EAX, EAX
	POP EAX
	JZ .END378
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t52], EAX ; store
	POP EAX ; store
.END378:
	PUSH EAX
	MOV EAX, 1
	MOV [t53], EAX ; store
	POP EAX ; store
	; IF ...
	MOV EAX, [t53]
	TEST EAX, EAX
	POP EAX
	JZ .END393
	; THEN ...
	PUSH EAX
	MOV EAX, 2
	MOV [t53], EAX ; store
	POP EAX ; store
.END393:
	RET

main:
	CALL T5
	PUSH EAX
	MOV EAX, 5416924
	MOV [ff], EAX ; store
	POP EAX ; store
	RET

.data
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
_t5:	dd 0
t50:	dd 0
t51:	dd 0
t52:	dd 0
t53:	dd 0
ff:	dd 0
