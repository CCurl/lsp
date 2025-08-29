
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
	; int num;
	; int x;
	; 
	; int T1() { }
;---------------------------------------------
T1:
	; int T2() { ; }
	RET
;---------------------------------------------
T2:
	; int T3() { if (1) {} }
	RET
;---------------------------------------------
T3:
IF_01:
	MOV 	EAX, 1
	TEST	EAX, EAX
	JZ  	ENDIF_01
THEN_01:
ENDIF_01:
	; int T4() { if (x == (num-1)) { x = num+1; } }
	RET
;---------------------------------------------
T4:
IF_02:
	MOV 	EAX, [x]
	MOV 	EBX, [num]
	MOV 	ECX, 1
	SUB 	EBX, ECX
	CMP 	EAX, EBX
	MOV 	EAX, 0
	JNE 	@F
	DEC 	EAX
@@:
	TEST	EAX, EAX
	JZ  	ENDIF_02
THEN_02:
	MOV 	EAX, [num]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	[x], EAX
ENDIF_02:
	; 
	; void SPC() { pv = ' '; putc(); }
	RET
;---------------------------------------------
SPC:
	MOV 	EAX, 32
	MOV 	[pv], EAX
	CALL	putc
	; void CR() { pv = 10; putc(); }
	RET
;---------------------------------------------
CR:
	MOV 	EAX, 10
	MOV 	[pv], EAX
	CALL	putc
	; void putCh(int chr) { pv=chr; putc(); }
	RET
;---------------------------------------------
putCh:
	MOV 	EAX, [EBP-4]
	MOV 	[pv], EAX
	CALL	putc
	; void putNum(int n) { pv=n; putd(); }
	RET
;---------------------------------------------
putNum:
	MOV 	EAX, [EBP-4]
	MOV 	[pv], EAX
	CALL	putd
	; 
	; int Mil(int m) {
	RET
;---------------------------------------------
Mil:
	; 	int c = 1000;
	MOV 	EAX, 1000
	MOV 	[EBP-8], EAX
	; 	x = m * (c * c);
	MOV 	EAX, [EBP-4]
	MOV 	EBX, [EBP-8]
	MOV 	ECX, [EBP-8]
	IMUL	EBX, ECX
	IMUL	EAX, EBX
	MOV 	[x], EAX
	; }
	; 
	; void main() {
	RET
;---------------------------------------------
main:
	; 	// putc(65);
	; 	putCh('s');
	MOV 	EAX, 115
	PUSH	EAX
	; 	x = 1000;
	CALL	x
	MOV 	EAX, 1000
	MOV 	[x], EAX
	; 	Mil(x, 1000);
	MOV 	EAX, [x]
	PUSH	EAX
	MOV 	EAX, 1000
	PUSH	EAX
	; 	putNum(x);
	CALL	putNum
	MOV 	EAX, [x]
	PUSH	EAX
	; 	num = x;
	CALL	num
	MOV 	EAX, [x]
	MOV 	[num], EAX
	; 	while (x) { x--; }
WHILE_01:
	MOV 	EAX, [x]
	TEST	EAX, EAX
	JZ  	WEND_01
	DEC 	[x]
	; 	putCh('e');
	JMP 	WHILE_01
WEND_01:
	MOV 	EAX, 101
	PUSH	EAX
	; }
	CALL	putCh
	; }
	RET

;================== data =====================
section '.data' data readable writeable
;=============================================

; symbols: 1000 entries, 19 used
; num type size name
; --- ---- ---- -----------------
pv        	dd 0
num       	dd 0
x         	dd 0
c         	dd 0

;====================================
section '.idata' import data readable
; ====================================
library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'
import msvcrt, printf,'printf', getch,'_getch'
import kernel32, ExitProcess,'ExitProcess'
