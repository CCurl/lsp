format PE console
include 'win32ax.inc'

;=======================================
section '.code' code readable executable
;=======================================
start: JMP main
       RET

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
	DEC		DWORD [x]
	JMP 	WHILE_01
WEND_01:
	RET

main:
		call bm
        cinvoke printf, "Enter a number: "
        cinvoke scanf, "%d", Number1
        cinvoke printf, "Enter a number: "
        cinvoke scanf, "%d", Number2

        mov ecx, [Number1]
        mov ebx, [Number2]
        add ebx, ecx
        mov [Sum], ebx
        cinvoke printf, "%d + %d = %d%c",[Number1],[Number2],[Sum],10
        cinvoke printf, "Press any key to close console..."
        invoke getch
        ret

;======================================
section '.data' data readable writeable
;======================================
Number1 dd 0
Number2 dd 0
Sum     dd 0
x		dd 0
num		dd 0

;====================================
section '.idata' import data readable
;====================================
library msvcrt,'msvcrt.dll',kernel32,'kernel32.dll'
import msvcrt,printf,'printf',scanf,'scanf',getch,'_getch' 
