; TC source file: test.tc
format PE console
include 'win32ax.inc'
;================== code =====================
.code
entry main
;================== library ==================
exit:	RET

putc:	RET

;=============================================
;---------------------------------------------
T1:
	MOV EAX, [x]
	MOV EBX, 1
	ADD EAX, EBX
	MOV EBX, 2
	SUB EAX, EBX
	RET
;---------------------------------------------
main:
	RET
;================== data =====================
.data
;=============================================
_pc_buf:	dd 0
x:	dd 0

; symbols: 1000 entries, 6 used
; num type size name
; --- ---- ---- -----------------
; 0   F    4    exit
; 1   F    4    putc
; 2   I    4    _pc_buf
; 3   F    4    T1
; 4   L    4    x
; 5   F    4    main
