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
;   1: 16  5   0     - DEF pN
;   2: 1   0   4     - LOADVAR EAX, [EBP+8]
;   3: 4   1   0     - STORE [pv], EAX
;   4: 17  4   0     - CALL putd
;   5: 19  0   0     - RETURN
pN:
	PUSH [EBP+8]
	POP DWORD [pv]
	CALL putd
	RET

;   1: 16  6   0     - DEF pC
;   2: 1   0   4     - LOADVAR EAX, [EBP+8]
;   3: 4   1   0     - STORE [pv], EAX
;   4: 17  3   0     - CALL putc
;   5: 19  0   0     - RETURN
pC:
	PUSH [EBP+8]
	POP DWORD [pv]
	CALL putc
	RET

;   1: 16  7   0     - DEF pS
;   2: 1   0   4     - LOADVAR EAX, [EBP+8]
;   3: 4   1   0     - STORE [pv], EAX
;   4: 17  2   0     - CALL puts
;   5: 19  0   0     - RETURN
pS:
	PUSH [EBP+8]
	POP DWORD [pv]
	CALL puts
	RET

;   1: 16  8   0     - DEF T4
;   2: 1   0   3     - LOADVAR EAX, [x]
;   3: 1   1   2     - LOADVAR EBX, [num]
;   4: 2   2   1     - LOADIMM ECX, 1
;   5: 6   1   2     - SUB EBX, ECX
;   6: 22  0   1     - CMP_EQ EAX, EBX
;   7: 13  4   0     - JMPZ .t1
;   8: 1   0   2     - LOADVAR EAX, [num]
;   9: 2   1   1     - LOADIMM EBX, 1
;  10: 5   0   1     - ADD EAX, EBX
;  11: 4   3   0     - STORE [x], EAX
;  12: 15  4   0     - TARGET .t1
;  13: 19  0   0     - RETURN
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

;   1: 16  9   0     - DEF Mil
;   2: 1   0   4     - LOADVAR EAX, [EBP+8]
;   3: 1   1   5     - LOADVAR EBX, [EBP+12]
;   4: 1   2   5     - LOADVAR ECX, [EBP+12]
;   5: 7   1   2     - MULT EBX, ECX
;   6: 7   0   1     - MULT EAX, EBX
;   7: 4   6   0     - STORE [EBP-4], EAX
;   8: 1   0   6     - LOADVAR EAX, [EBP-4]
;   9: 19  1   1     - RETURN
;  10: 19  0   1     - RETURN
Mil:
	PUSH [EBP+8]
	PUSH [EBP+12]
	PUSH [EBP+12]
	POP EBX
	POP EAX
	IMUL EAX, EBX
	PUSH EAX
	POP EBX
	POP EAX
	IMUL EAX, EBX
	PUSH EAX
	POP DWORD [EBP-4]
	PUSH [EBP-4]
	RET
	RET

;   1: 16  10  0     - DEF main
;   2: 2   0   115   - LOADIMM EAX, 115
;   3: 4   4   5     - STORE [EBP-4], EAX
;   4: 1   0   4     - LOADVAR EAX, [EBP-4]
;   5: 18  1   2     - PARAM EAX
;   6: 17  6   1     - CALL pC
;   7: 2   0   999   - LOADIMM EAX, 999
;   8: 4   3   6     - STORE [x], EAX
;   9: 2   0   500   - LOADIMM EAX, 500
;  10: 18  0   1     - PARAM EAX
;  11: 2   0   1000  - LOADIMM EAX, 1000
;  12: 18  4   0     - PARAM EAX
;  13: 17  9   0     - CALL Mil
;  14: 1   0   3     - LOADVAR EAX, [x]
;  15: 18  0   0     - PARAM EAX
;  16: 17  5   0     - CALL pN
;  17: 1   0   3     - LOADVAR EAX, [x]
;  18: 4   2   0     - STORE [num], EAX
;  19: 15  5   0     - TARGET .t2
;  20: 1   0   3     - LOADVAR EAX, [x]
;  21: 13  6   0     - JMPZ .t3
;  22: 25  3   0     - DECVAR [x]
;  23: 12  5   0     - JMP .t2
;  24: 15  6   0     - TARGET .t3
;  25: 2   0   101   - LOADIMM EAX, 101
;  26: 4   4   0     - STORE [EBP-4], EAX
;  27: 1   0   4     - LOADVAR EAX, [EBP-4]
;  28: 18  0   0     - PARAM EAX
;  29: 17  6   0     - CALL pC
;  30: 3   0   1     - LOADSTR EAX, [pv]
;  31: 18  0   0     - PARAM EAX
;  32: 17  7   0     - CALL pS
;  33: 17  1   0     - CALL exit
;  34: 19  0   0     - RETURN
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
	LEA EAX, [_str1]
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
