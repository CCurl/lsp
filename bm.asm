
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
	;      int num;
	;      int x;
	;      
	;      int T1() { }
;---------------------------------------------
T1:
	;      int T2() { ; }
	RET
;---------------------------------------------
T2:
	;      int T3() { if (1) {} }
	RET
;---------------------------------------------
T3:
IF_01:
	MOV 	EAX, 1
	TEST	EAX, EAX
	JZ  	ENDIF_01
THEN_01:
ENDIF_01:
	;      int T4() { if (x == (num-1)) { x = num+1; } }
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
	INC 	EAX
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
	;      int Mil() {
	RET
;---------------------------------------------
Mil:
	;      	x = x * (1000 * 1000);
	MOV 	EAX, [x]
	MOV 	EBX, 1000
	MOV 	ECX, 1000
	IMUL	EBX, ECX
	IMUL	EAX, EBX
	MOV 	[x], EAX
	;      }
	;      
	;      void main() {
	RET
;---------------------------------------------
main:
	;      	// putc(65);
	;      	int c;
	;      	c = 's';
	MOV 	EAX, 115
	MOV 	[c], EAX
	;      	x = 1000;
	MOV 	EAX, 1000
	MOV 	[x], EAX
	;      	Mil();
	CALL	Mil
	;      	num = x;
	MOV 	EAX, [x]
	MOV 	[num], EAX
	;      	while (x) { x--; }
WHILE_01:
	MOV 	EAX, [x]
	TEST	EAX, EAX
	JZ  	WEND_01
	DEC 	[x]
	;      	c = 'e';
	JMP 	WHILE_01
WEND_01:
	MOV 	EAX, 101
	MOV 	[c], EAX
	;      	// putc(66);
	;      }
	;      }
	RET

;================== data =====================
section '.data' data readable writeable
;=============================================

; symbols: 1000 entries, 12 used
; num type size name
; --- ---- ---- -----------------
_pc_buf		dd 0
num		dd 0
x		dd 0
c		dd 0

;====================================
section '.idata' import data readable
; ====================================
library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'
import msvcrt,printf,'printf',scanf,'scanf',getch,'_getch'
