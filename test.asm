format PE console
include 'win32ax.inc'

; ======================================= 
section '.code' code readable executable
;=======================================*/

start: JMP main
;================== library ==================
exit:
	PUSH 0
	CALL [ExitProcess]

puts:
	cinvoke printf, "%s", [pv]
	RET

putc:
	cinvoke printf, "%c", [pv]
	RET

putd:
	cinvoke printf, "%d", [pv]
	RET

;=============================================
pN:
	PUSH [EBP+8]
	POP DWORD [pv]
	CALL putd
	RET

pC:
	PUSH [EBP+8]
	POP DWORD [pv]
	CALL putc
	RET

pS:
	PUSH [EBP+8]
	POP DWORD [pv]
	CALL puts
	RET

T4:
	PUSH [x]
	PUSH [num]
	PUSH 1
	POP EAX
	SUB [ESP], EAX
	CMP_EQ EAX, EBX
	JMPZ .t1
	PUSH [num]
	PUSH 1
	POP EAX
	ADD [ESP], EAX
	POP DWORD [x]
.t1:
	RET

Mil:
	PUSH [EBP+8]
	PUSH [EBP+12]
	PUSH [EBP+12]
	POP EAX
	POP EBX
	IMUL EAX, EBX
	PUSH EAX
	POP EAX
	POP EBX
	IMUL EAX, EBX
	PUSH EAX
	POP DWORD [EBP-4]
	PUSH [EBP-4]
	RET
	RET

main:
	PUSH 115
	POP DWORD [EBP-4]
	PUSH [EBP-4]
	; PARAM
	CALL pC
	PUSH 999
	POP DWORD [x]
	PUSH 500
	; PARAM
	PUSH 1000
	; PARAM
	CALL Mil
	PUSH [x]
	; PARAM
	CALL pN
	PUSH [x]
	POP DWORD [num]
.t2:
	PUSH [x]
	JMPZ .t3
	DEC DWORD [x]
	JMP .t2
.t3:
	PUSH 101
	POP DWORD [EBP-4]
	PUSH [EBP-4]
	; PARAM
	CALL pC
	LEA EAX, []
	PUSH EAX
	; PARAM
	CALL pS
	CALL exit
	RET


;================== data =====================
section '.data' data readable writeable
;=============================================

; symbols: 100 entries, 3 used
; num type size name
; --- ---- ---- -----------------
pv        	dd 0
num       	dd 0
x         	dd 0
_str1     	db " - all done!", 0

;====================================
section '.idata' import data readable
; ====================================
library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'
import msvcrt, printf,'printf', getch,'_getch'
import kernel32, ExitProcess,'ExitProcess'
