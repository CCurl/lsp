format PE console
include 'win32ax.inc'

;=======================================
section '.code' code readable executable
;=======================================
start: JMP main
       RET

;---------------------------------------------
	POP EAX
	POP EBX
	POP DWORD [EBP+8]
	MOV [EAX], EBX
	RET
;---------------------------------------------
DPOP:
	XCHG ESP, EBP
	ADD EBP, 4
	MOV [EBP], EAX
	POP DWORD [x]
	XCHG ESP, EBP
	RET
;---------------------------------------------
Mil:
	MOV 	EAX, [x]
	MOV 	EBX, 1000
	MOV 	ECX, 1000
	IMUL	EBX, ECX
	IMUL	EAX, EBX
	CDQ
	IDIV	EBX
	MOV 	[x], EAX
	RET

;---------------------------------------------
bm:
	MOV 	EAX, 500
	MOV 	[x], EAX
	CALL	Mil
	MOV 	EAX, [x]
	MOV 	[num], EAX
WHILE_01:
	MOV 	EAX, [x]
	TEST	EAX, EAX
	JZ  	WEND_01
	DEC 	[x]
	JMP 	WHILE_01
WEND_01:
	RET

;---------------------------------------------
main:
		cinvoke printf, "Start ..."
		call bm
		cinvoke printf, "End."
        cinvoke printf, "%cEnter a number: ", 10
        cinvoke scanf, "%d", Number1
        cinvoke printf, "Enter a number: "
        cinvoke scanf, "%d", Number2

		push [x]
		push 1
		pop eax
		pop ebx

		POP EAX
		ADD DWORD [ESP], EAX

		POP EAX
		SUB DWORD [ESP], EAX

        mov ecx, [Number1]
        mov ebx, [Number2]
        add ebx, ecx
        mov [Sum], ebx
        cinvoke printf, "%d + %d = %d%c", [Number1],[Number2],[Sum],10
        cinvoke printf, "%s", xxx
        ; cinvoke printf, "Press any key to close console..."
        ; invoke getch
        ret

;======================================
section '.data' data readable writeable
;======================================
Number1 dd 0
Number2 dd 0
Sum     dd 0
x		dd 0
num		dd 0
xxx		db "hi there", 0

;====================================
section '.idata' import data readable
;====================================
library msvcrt,'msvcrt.dll',kernel32,'kernel32.dll'
import msvcrt,printf,'printf',scanf,'scanf',getch,'_getch' 
