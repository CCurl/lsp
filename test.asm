; TC source file: test.tc
format PE console
include 'win32ax.inc'

; ======================================= 
section '.code' code readable executable
;=======================================*/

start: JMP main
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
IF_01:
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
	JZ  	ENDIF_01
THEN_01:
	MOV 	EAX, [y]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	[y], EAX
ENDIF_01:
	MOV 	EAX, 1
	MOV 	[z], EAX
IF_02:
	MOV 	EAX, [y]
	MOV 	EBX, 5
	CMP 	EAX, EBX
	MOV 	EAX, 0
	JNE 	@F
	INC 	EAX
@@:
	TEST	EAX, EAX
	JZ  	ENDIF_02
THEN_02:
	MOV 	EAX, [y]
	MOV 	EBX, 3
	IMUL	EAX, EBX
	MOV 	[y], EAX
ENDIF_02:
	MOV 	EAX, [z]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	[z], EAX
IF_03:
	MOV 	EAX, [y]
	MOV 	EBX, 5
	CMP 	EAX, EBX
	MOV 	EAX, 0
	JLE 	@F
	INC 	EAX
@@:
	TEST	EAX, EAX
	JZ  	ENDIF_03
THEN_03:
	MOV 	EAX, [y]
	MOV 	EBX, 1
	SUB 	EAX, EBX
	MOV 	[y], EAX
ENDIF_03:
	RET
	RET
;---------------------------------------------
main:
	MOV 	EAX, 10
	MOV 	EBX, 1000
	IMUL	EAX, EBX
	MOV 	EBX, 1000
	IMUL	EAX, EBX
	MOV 	[m1], EAX
WHILE_01:
	MOV 	EAX, [m1]
	TEST	EAX, EAX
	JZ  	WEND_01
	DEC 	[m1]
	JMP 	WHILE_01
WEND_01:
	RET
	RET

;================== data =====================
section '.data' data readable writeable
;=============================================

; symbols: 1000 entries, 10 used
; num type size name
; --- ---- ---- -----------------
_pc_buf		dd 0
zz		dd 0
x		dd 0
y		dd 0
z		dd 0
m1		dd 0


;====================================
section '.idata' import data readable
; ====================================
library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'
import msvcrt,printf,'printf',scanf,'scanf',getch,'_getch'
