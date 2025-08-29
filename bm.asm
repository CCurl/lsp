
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
-expected token [11], not[27]-
; syntax error at(21, 11)
;  putCh('s');
          ^