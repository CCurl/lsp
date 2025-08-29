
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
	; int zz;
	; int yy[10];
	; 
	; //int T0(a,b,c) {
	; //	return a+b+c;
	; //}
	; 
	; int T1() {
;---------------------------------------------
T1:
	; 	int x;
	; 	int y;
	; 	x = 1-(y*(10/x)-4);
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
	; 	y = x/23;
	MOV 	EAX, [x]
	MOV 	EBX, 23
	XCHG	ECX, EBX
	CDQ
	IDIV	ECX
	MOV 	[y], EAX
	; 	if ((y+1) < (x*2)) { y = y+1; }
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
	DEC 	EAX
@@:
	TEST	EAX, EAX
	JZ  	ENDIF_01
THEN_01:
	MOV 	EAX, [y]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	[y], EAX
	; 	int z; z = 1;
ENDIF_01:
	MOV 	EAX, 1
	MOV 	[z], EAX
	; 	if (y == 5) { y = y*3; }
IF_02:
	MOV 	EAX, [y]
	MOV 	EBX, 5
	CMP 	EAX, EBX
	MOV 	EAX, 0
	JNE 	@F
	DEC 	EAX
@@:
	TEST	EAX, EAX
	JZ  	ENDIF_02
THEN_02:
	MOV 	EAX, [y]
	MOV 	EBX, 3
	IMUL	EAX, EBX
	MOV 	[y], EAX
	; 	z = z+1;
ENDIF_02:
	MOV 	EAX, [z]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	[z], EAX
	; 	if (y > 5) { y = y-1; }
IF_03:
	MOV 	EAX, [y]
	MOV 	EBX, 5
	CMP 	EAX, EBX
	MOV 	EAX, 0
	JLE 	@F
	DEC 	EAX
@@:
	TEST	EAX, EAX
	JZ  	ENDIF_03
THEN_03:
	MOV 	EAX, [y]
	MOV 	EBX, 1
	SUB 	EAX, EBX
	MOV 	[y], EAX
	; 	z = x&y;
ENDIF_03:
	MOV 	EAX, [x]
	MOV 	EBX, [y]
	AND 	EAX, EBX
	MOV 	[z], EAX
	; 	z = x|y;
	MOV 	EAX, [x]
	MOV 	EBX, [y]
	OR  	EAX, EBX
	MOV 	[z], EAX
	; 	z = x^y;
	MOV 	EAX, [x]
	MOV 	EBX, [y]
	XOR 	EAX, EBX
	MOV 	[z], EAX
	; 	return x+y;
	MOV 	EAX, [x]
	MOV 	EBX, [y]
	ADD 	EAX, EBX
	; }
	RET
	; 
	; void Mil() {
	RET
;---------------------------------------------
Mil:
	; 	zz = zz * 1000 * 1000;
	MOV 	EAX, [zz]
	MOV 	EBX, 1000
	IMUL	EAX, EBX
	MOV 	EBX, 1000
	IMUL	EAX, EBX
	MOV 	[zz], EAX
	; }
	; 
	; void main() {
	RET
;---------------------------------------------
main:
	; 	int m1;
	; 	m1 = 's';
	MOV 	EAX, 115
	MOV 	[m1], EAX
	; 	zz = 1000; Mil();
	MOV 	EAX, 1000
	MOV 	[zz], EAX
	PUSH	EBP
	MOV 	EBP, ESP
	; 	m1 = zz;
	CALL	Mil
	MOV 	ESP, EBP
	POP 	EBP
	MOV 	EAX, [zz]
	MOV 	[m1], EAX
	; 	pv = "start ... "; puts();
	LEA 	EAX, [_s001_]
	MOV 	[pv], EAX
	PUSH	EBP
	MOV 	EBP, ESP
	; 	while (m1) { m1--; }
	CALL	puts
	MOV 	ESP, EBP
	POP 	EBP
WHILE_01:
	MOV 	EAX, [m1]
	TEST	EAX, EAX
	JZ  	WEND_01
	DEC 	[m1]
	; 	pv = "end."; puts();
	JMP 	WHILE_01
WEND_01:
	LEA 	EAX, [_s002_]
	MOV 	[pv], EAX
	PUSH	EBP
	MOV 	EBP, ESP
	; 	pv = 10; putc();
	CALL	puts
	MOV 	ESP, EBP
	POP 	EBP
	MOV 	EAX, 10
	MOV 	[pv], EAX
	PUSH	EBP
	MOV 	EBP, ESP
	; 	pv = 123; putd();
	CALL	putc
	MOV 	ESP, EBP
	POP 	EBP
	MOV 	EAX, 123
	MOV 	[pv], EAX
	PUSH	EBP
	MOV 	EBP, ESP
	; 	int m2;
	CALL	putd
	MOV 	ESP, EBP
	POP 	EBP
	; 	m2 += m1;
	MOV 	EAX, [m1]
	ADD 	[m2], EAX
	; 	return;
	; }
	RET
	; }
	RET

;================== data =====================
section '.data' data readable writeable
;=============================================

; symbols: 1000 entries, 10 used
; num type size name
; --- ---- ---- -----------------
pv        	dd 1 DUP(0)
zz        	dd 1 DUP(0)
yy        	dd 10 DUP(0)
x         	dd 0
y         	dd 0
z         	dd 0
m1        	dd 0
_s001_    	db "start ... ",0
_s002_    	db "end.",0
m2        	dd 0

;====================================
section '.idata' import data readable
; ====================================
library msvcrt, 'msvcrt.dll', kernel32, 'kernel32.dll'
import msvcrt, printf,'printf', getch,'_getch'
import kernel32, ExitProcess,'ExitProcess'
