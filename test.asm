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
	; int pN(int c) { pv = c; putd(); }
;---------------------------------------------
pN:
	PUSH	EBP
	MOV 	EBP, ESP
	SUB 	ESP, 32
	MOV 	EAX, [EBP+8]
	MOV 	[pv], EAX
	CALL	putd
	; int pC(int c) { pv = c; putc(); }
.RET:
	MOV 	ESP, EBP
	POP 	EBP
	RET
;---------------------------------------------
pC:
	PUSH	EBP
	MOV 	EBP, ESP
	SUB 	ESP, 32
	MOV 	EAX, [EBP+8]
	MOV 	[pv], EAX
	CALL	putc
	; int pS(int c) { pv = c; puts(); }
.RET:
	MOV 	ESP, EBP
	POP 	EBP
	RET
;---------------------------------------------
pS:
	PUSH	EBP
	MOV 	EBP, ESP
	SUB 	ESP, 32
	MOV 	EAX, [EBP+8]
	MOV 	[pv], EAX
	CALL	puts
	; int T4() { if (x == (num-1)) { x = num+1; } }
.RET:
	MOV 	ESP, EBP
	POP 	EBP
	RET
;---------------------------------------------
T4:
	PUSH	EBP
	MOV 	EBP, ESP
	SUB 	ESP, 32
IF_01:
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
	JZ  	ENDIF_01
THEN_01:
	MOV 	EAX, [num]
	MOV 	EBX, 1
	ADD 	EAX, EBX
	MOV 	[x], EAX
ENDIF_01:
	; 
	; int Mil(int m, int c) {
.RET:
	MOV 	ESP, EBP
	POP 	EBP
	RET
;---------------------------------------------
Mil:
	PUSH	EBP
	MOV 	EBP, ESP
	SUB 	ESP, 32
	; 	int x = m * (c * c);
	MOV 	EAX, [EBP+8]
	MOV 	EBX, [EBP+12]
	MOV 	ECX, [EBP+12]
	IMUL	EBX, ECX
	IMUL	EAX, EBX
	MOV 	[x], EAX
	; 	return x;
	MOV 	EAX, [x]
	; }
	JMP .RET
	; 
	; void main() {
.RET:
	MOV 	ESP, EBP
	POP 	EBP
	RET
;---------------------------------------------
main:
	PUSH	EBP
	MOV 	EBP, ESP
	SUB 	ESP, 32
	; 	// putc(65);
	; 	int c = 's';
	MOV 	EAX, 115
	MOV 	[EBP+12], EAX
	; 	pC(c);
	MOV 	EAX, [EBP+12]
	PUSH	EAX
	CALL	pC
	; 	x = 999;
	MOV 	EAX, 999
	MOV 	[x], EAX
	; 	Mil(1000, 500);
	MOV 	EAX, 1000
	PUSH	EAX
	MOV 	EAX, 500
	PUSH	EAX
	CALL	Mil
	; 	pN(x);
	MOV 	EAX, [x]
	PUSH	EAX
	CALL	pN
	; 	num = x;
	MOV 	EAX, [x]
	MOV 	[num], EAX
	; 	while (x) { x--; }
WHILE_01:
	MOV 	EAX, [x]
	TEST	EAX, EAX
	JZ  	WEND_01
	DEC 	[x]
	; 	c = 'e';
	JMP 	WHILE_01
WEND_01:
syntax error at(23, 3)
 c = 'e';
 ^