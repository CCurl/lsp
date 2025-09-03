format ELF executable
;================== code =====================
segment readable executable
entry main
;================== library ==================
init:
	LEA EDI, [stk]
	ADD EDI, 1024
	RET


exit:
	MOV EAX, 1
	XOR EBX, EBX
	INT 0x80

puts:
	MOV [pv], EAX
	RET
putd:
	MOV [pv], EAX
	RET
putc:
	MOV [pv], EAX
	MOV EAX, 4
	MOV EBX, 0
	LEA ECX, [pv]
	MOV EDX, 1
	INT 0x80
	RET

;=============================================
; -x/0/0-
; -@/0/0-
; -num/0/0-
; -@/0/0-
; -1-/0/1-
; -=/0/0-
; -if/0/0-
; -num/0/0-
; -@/0/0-
; -1+/0/1-
; -x/0/0-
; -!/0/0-
; -then/0/0-
; -;/0/0-
; -///0/0-
; -n/0/0-
; ---/0/0-
; -n'/0/0-
; -1000/1/1000-
; -*/0/0-
; -1000/1/1000-
; -*/0/0-
; -;/0/0-
; -init/0/0-
; -'s'/0/0-
; -putc/0/0-
; -500/1/500-
; -Mil/0/0-
; -x/0/0-
; -!/0/0-
; -x/0/0-
; -@/0/0-
; -begin/0/0-
; -1-/0/1-
; -while/0/0-
; -drop/0/0-
; -'e'/0/0-
; -putc/0/0-
; -"/0/0-
; --/0/0-
; -all/0/0-
; -done!"/0/0-
; -puts/0/0-
; -bye/0/0-
; -;/0/0-
; -/0/0-
_v_5:
	CALL _v_4 ; x (4)
	CALL  ;  (0)
	CALL _v_3 ; num (3)
	CALL  ;  (0)
	DEC []
	CMP_GT EAX, EAX
	CALL _v_3 ; num (3)
	CALL  ;  (0)
	INC []
	CALL _v_4 ; x (4)
	CALL  ;  (0)
	CALL  ;  (0)
	RET
	CALL  ;  (0)
	CALL  ;  (0)
	CALL  ;  (0)
	CALL  ;  (0)
_v_8:
	PUSH 1000
	POP EBX
	POP EAX
	IMUL EAX, EBX
	PUSH EAX
	PUSH 1000
	POP EBX
	POP EAX
	IMUL EAX, EBX
	PUSH EAX
	RET
_v_9:
	CALL  ;  (0)
	CALL  ;  (0)
	CALL  ;  (0)
	PUSH 500
	CALL _v_8 ; Mil (8)
	CALL _v_6 ; x (6)
	CALL  ;  (0)
	CALL _v_6 ; x (6)
	CALL  ;  (0)
	DEC []
	CALL  ;  (0)
	CALL  ;  (0)
	CALL  ;  (0)
	CALL  ;  (0)
	CALL  ;  (0)
	POP EAX
	SUB [ESP], EAX
	CALL  ;  (0)
	CALL  ;  (0)
	CALL  ;  (0)
	CALL  ;  (0)
	RET
	CALL  ;  (0)
;================== data =====================
segment readable writeable
;=============================================

; symbols: 100 entries, 9 used
; num type size name
; --- ---- ---- -----------------
_v_2       dd 0 ; pv
_v_3       dd 0 ; num
_v_4       dd 0 ; x
_v_6       dd 0 ; x
_v_7       dd 0 ; y
stk        rd 256
