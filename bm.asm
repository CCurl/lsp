; TC source file: bm.tc
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
TMil:
	RET
	RET
;---------------------------------------------
Mil:
	MOV 	EAX, [x]
	MOV 	EBX, 1001
	MOV 	ECX, 1
	SUB 	EBX, ECX
	IMUL	EAX, EBX
	MOV 	EBX, 999
	MOV 	ECX, 1
	ADD 	EBX, ECX
	IMUL	EAX, EBX
	MOV 	[x], EAX
	RET
;---------------------------------------------
main:
	MOV 	EAX, 500
	MOV 	EBX, 1000
	IMUL	EAX, EBX
	MOV 	EBX, 1000
	IMUL	EAX, EBX
	MOV 	[x], EAX
	MOV 	EAX, [x]
	MOV 	[num], EAX
WHILE_01:
	MOV 	EAX, [x]
	TEST	EAX, EAX
	JZ  	WEND_01
	MOV 	EAX, [x]
	MOV 	EBX, 1
	SUB 	EAX, EBX
	MOV 	[x], EAX
	JMP 	WHILE_01
WEND_01:
	RET

;================== data =====================
section '.data' data readable writeable
;=============================================

; symbols: 1000 entries, 8 used
; num type size name
; --- ---- ---- -----------------
_pc_buf		dd 0
num		dd 0
x		dd 0


;====================================
section '.idata' import data readable
; ====================================
library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'
import msvcrt,printf,'printf',scanf,'scanf',getch,'_getch'
