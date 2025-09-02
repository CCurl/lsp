format PE console
include 'win32ax.inc'

; ======================================= 
section '.code' code readable executable
;=======================================*/
start:
	CALL init
	JMP F13

;================== library ==================
F1:
	PUSH 0
	CALL [ExitProcess]

F2: ; puts
	CALL RETtoEBP
	MOV [I5], EAX
	POP EAX
	cinvoke printf, "%s", [I5]
	CALL RETfromEBP

F3: ; emit
	CALL RETtoEBP
	MOV [I5], EAX
	POP EAX
	cinvoke printf, "%c", [I5]
	CALL RETfromEBP

F4: ; .d
	CALL RETtoEBP
	MOV [I5], EAX
	POP EAX
	cinvoke printf, "%d", [I5]
	CALL RETfromEBP

;=============================================
init:
	LEA EBP, [rstk]
	RET
; Move the return addr to the [EBP] stack
; NB: EDX is destroyed
RETtoEBP:
	POP  EDX
	ADD  EBP, 4
	POP  DWORD [EBP]
	PUSH EDX
	RET

; Restore the return addr from the [EBP] stack
; NB: EDX is destroyed
EBPtoRET:
	POP  EDX
	PUSH DWORD [EBP]
	SUB  EBP, 4
	PUSH EDX
	RET

; Perform a RET from the [EBP] stack
; NB: EDX is destroyed
RETfromEBP:
	POP  EDX
	PUSH DWORD [EBP]
	SUB  EBP, 4
	RET

F8: ; T4
	CALL RETtoEBP
	PUSH EAX
	LEA EAX, [I7] ; x
	MOV EAX, [EAX]
	PUSH EAX
	LEA EAX, [I6] ; num
	MOV EAX, [EAX]
	DEC EAX
	POP EBX
	CMP EBX, EAX
	MOV EAX, 0
	JLE @F
	DEC EAX
@@:
	TEST EAX, EAX
	JZ .t1
	PUSH EAX
	LEA EAX, [I6] ; num
	MOV EAX, [EAX]
	INC EAX
	PUSH EAX
	LEA EAX, [I7] ; x
	POP ECX
	MOV [EAX], ECX
.t1:
	CALL RETfromEBP

F12: ; Mil
	CALL RETtoEBP
	PUSH EAX
	MOV EAX,1000
	POP EBX
	IMUL EAX, EBX
	PUSH EAX
	MOV EAX,1000
	POP EBX
	IMUL EAX, EBX
	CALL RETfromEBP

F13: ; main
	CALL RETtoEBP
	PUSH EAX
	MOV EAX,1000
	CALL F12 ; Mil (12)
	PUSH EAX
	LEA EAX, [I10] ; x
	POP ECX
	MOV [EAX], ECX
	PUSH EAX
	LEA EAX, [I10] ; x
	MOV EAX, [EAX]
	CALL F4 ; .d (4)
	PUSH EAX
	MOV EAX,115
	CALL F3 ; emit (3)
	PUSH EAX
	LEA EAX, [I10] ; x
	MOV EAX, [EAX]
.t2:
	DEC EAX
	TEST EAX, EAX
	JNZ .t2
	PUSH EAX
	MOV EAX,101
	CALL F3 ; emit (3)
	PUSH EAX
	LEA EAX, [S1]
	CALL F2 ; puts (2)
	CALL F1 ; bye (1)
	CALL RETfromEBP

;================== data =====================
section '.data' data readable writeable
;=============================================

; symbols: 100 entries, 14 used
; num type size name
; --- ---- ---- -----------------
I5         dd 0 ; pv
I6         dd 0 ; num
I7         dd 0 ; x
I10        dd 0 ; x
I11        dd 0 ; y
S1         db "- all done!", 0
rstk       rd 256

;====================================
section '.idata' import data readable
; ====================================
library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'
import msvcrt, printf,'printf', getch,'_getch'
import kernel32, ExitProcess,'ExitProcess'
