format ELF executable
segment readable writable executable
entry main
; source: test.tc
T1:
	MOV EAX, 11111
	MOV [_t1], EAX
	MOV EAX, 1
	MOV [t13], EAX
	MOV EAX, 17
	MOV EAX, 8
	MOV EBX, EAX
	POP EAX
	idiv eax
	MOV EAX, 2
	MOV EAX, EBX
	; jz .jz
	MOV EAX, [t13]
	MOV EAX, 1
	POP EBX
	add eax, ebx
	MOV [t13], EAX
	MOV EAX, 1
	MOV [t11], EAX
	MOV EAX, 2
	MOV EAX, 3
	MOV EAX, EBX
	MOV EAX, 3
	MOV EAX, 4
	MOV EAX, EBX
	POP EBX
	and eax, ebx
	MOV EAX, 0
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t11], EAX
	MOV EAX, 1
	MOV [t12], EAX
	MOV EAX, 2
	MOV EAX, 3
	MOV EAX, EBX
	MOV EAX, 3
	MOV EAX, 4
	MOV EAX, EBX
	POP EBX
	or eax, ebx
	MOV EAX, 0
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t12], EAX
	RET

T2:
	CALL T1
	MOV EAX, 22222
	MOV [_t2], EAX
	MOV EAX, 1
	MOV [t2], EAX
	MOV EAX, 0
	MOV [c], EAX
.L62:
	MOV EAX, [t2]
	MOV EAX, 1025
	MOV EAX, EBX
	; jz .jz
	MOV EAX, [c]
	MOV EAX, 1
	POP EBX
	add eax, ebx
	MOV [c], EAX
	MOV EAX, [t2]
	MOV EAX, 2
	POP EBX
	imul eax
	MOV [t2], EAX
	JMP .L62
	MOV EAX, 1
	MOV [t21], EAX
	MOV EAX, [t2]
	MOV EAX, 2047
	MOV EAX, [t21]
	POP EBX
	add eax, ebx
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t21], EAX
	MOV EAX, 1
	MOV [t22], EAX
	MOV EAX, [c]
	MOV EAX, 11
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t22], EAX
	RET

T3:
	CALL T2
	MOV EAX, 33333
	MOV [_t3], EAX
	MOV EAX, 1
	MOV [t30], EAX
	MOV EAX, 8
	MOV EAX, 1
	POP EBX
	add eax, ebx
	MOV EAX, 12
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	sub eax, ebx
	POP EBX
	imul eax
	MOV EAX, 100
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	sub eax, ebx
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t30], EAX
	RET

T4:
	CALL T3
	MOV EAX, 44444
	MOV [_t4], EAX
	MOV EAX, 0
	MOV [t40], EAX
.L133:
	MOV EAX, [t40]
	MOV EAX, 10
	MOV EAX, EBX
	; jz .jz
	MOV EAX, [t40]
	MOV EAX, 1
	POP EBX
	add eax, ebx
	MOV [t40], EAX
	JMP .L133
	MOV EAX, [t40]
	MOV EAX, 1
	MOV EBX, EAX
	POP EAX
	sub eax, ebx
	MOV [t40], EAX
	MOV EAX, [t40]
	MOV EAX, 5
	MOV EAX, EBX
	; jnz .jnz
	MOV EAX, [t40]
	MOV EAX, 5
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t40], EAX
	RET

T5:
	CALL T4
	MOV EAX, 55555
	MOV [_t5], EAX
	MOV EAX, 1
	MOV [t50], EAX
	MOV EAX, 1
	MOV EAX, 2
	POP EBX
	and eax, ebx
	MOV EAX, 0
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t50], EAX
	MOV EAX, 1
	MOV [t51], EAX
	MOV EAX, 1
	MOV EAX, 2
	POP EBX
	or eax, ebx
	MOV EAX, 3
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t51], EAX
	MOV EAX, 1
	MOV [t52], EAX
	MOV EAX, 3
	MOV EAX, 1
	POP EBX
	xor eax, ebx
	MOV EAX, 2
	MOV EAX, EBX
	; jz .jz
	MOV EAX, 2
	MOV [t52], EAX
	MOV EAX, 1
	MOV [t53], EAX
	MOV EAX, [t53]
	; jz .jz
	MOV EAX, 2
	MOV [t53], EAX
	RET

main:
	CALL T5
	MOV EAX, 1448939988
	MOV [ff], EAX
	RET

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
