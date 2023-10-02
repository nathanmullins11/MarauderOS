bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call            ; The C function that sys_call_isr will call
sys_call_isr:
	cmp EAX, 1
	je equal_to_one

	jmp end_if


	equal_to_one: 
	; push reverse order of struct
	push ESP
	push EBP
	push EDI
	push ESI
	push EDX
	push ECX
	push EBX
	push EAX
	push SS
	push GS
	push FS
	push ES
	push DS
	
	; push stack pointer
	;; push ESP

	; call function
	call sys_call

	; set stack pointer based on return value 
	mov ESP, EAX

	; pop same order as struct

	;; pop CS
	pop DS
	pop ES
	pop FS
	pop GS
	pop SS


	pop EAX
	pop EBX
	pop ECX
	pop EDX
	pop ESI
	pop EDI
	pop EBP
	pop ESP

	; mov EAX, 0
	iret
	jmp end_if

	end_if:
	mov EAX, -1
	iret