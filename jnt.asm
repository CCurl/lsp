; code: 25000 entries, 14 used

F2: ; main
	MOV  EAX, 1
	ADD  EAX, [I0] ; Abc
	SUB  EAX, 3
	MOV  [A], EAX
	MOV  EAX, [A]
	IMUL EAX, [B]
	IDIV [C]
	MOV  [B], EAX
	RET

; symbols: 1000 entries, 3 used
; ------------------------------------
I0        	dd 0 ; Abc
C1        	db 256 dup(0) ; Def
; F2 ; Function main
A         	dd 0
A_Sp      	dd 0
AStk      	dd 32 dup(0)
B         	dd 0
B_Sp      	dd 0
BStk      	dd 32 dup(0)
C         	dd 0
C_Sp      	dd 0
CStk      	dd 32 dup(0)
D         	dd 0
D_Sp      	dd 0
DStk      	dd 32 dup(0)
E         	dd 0
E_Sp      	dd 0
EStk      	dd 32 dup(0)
F         	dd 0
F_Sp      	dd 0
FStk      	dd 32 dup(0)
G         	dd 0
G_Sp      	dd 0
GStk      	dd 32 dup(0)
H         	dd 0
H_Sp      	dd 0
HStk      	dd 32 dup(0)
I         	dd 0
I_Sp      	dd 0
IStk      	dd 32 dup(0)
J         	dd 0
J_Sp      	dd 0
JStk      	dd 32 dup(0)
K         	dd 0
K_Sp      	dd 0
KStk      	dd 32 dup(0)
L         	dd 0
L_Sp      	dd 0
LStk      	dd 32 dup(0)
M         	dd 0
M_Sp      	dd 0
MStk      	dd 32 dup(0)
N         	dd 0
N_Sp      	dd 0
NStk      	dd 32 dup(0)
O         	dd 0
O_Sp      	dd 0
OStk      	dd 32 dup(0)
P         	dd 0
P_Sp      	dd 0
PStk      	dd 32 dup(0)
Q         	dd 0
Q_Sp      	dd 0
QStk      	dd 32 dup(0)
R         	dd 0
R_Sp      	dd 0
RStk      	dd 32 dup(0)
S         	dd 0
S_Sp      	dd 0
SStk      	dd 32 dup(0)
T         	dd 0
T_Sp      	dd 0
TStk      	dd 32 dup(0)
U         	dd 0
U_Sp      	dd 0
UStk      	dd 32 dup(0)
V         	dd 0
V_Sp      	dd 0
VStk      	dd 32 dup(0)
W         	dd 0
W_Sp      	dd 0
WStk      	dd 32 dup(0)
X         	dd 0
X_Sp      	dd 0
XStk      	dd 32 dup(0)
Y         	dd 0
Y_Sp      	dd 0
YStk      	dd 32 dup(0)
Z         	dd 0
Z_Sp      	dd 0
ZStk      	dd 32 dup(0)