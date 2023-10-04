bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call
sys_call_isr:

	; push reverse order of struct

	cmp EAX, 1
	je equal_to_one

	jmp end_if

	equal_to_one:

	push SS
	push GS
	push FS
	push ES
	push DS

	pusha

	; call function
	call sys_call

	; set stack pointer based on return value 
	mov ESP, EAX

	pop DS
	pop ES
	pop FS
	pop GS
	pop SS

	popa

	iret

	jmp end_if

	end_if:
	mov EAX, -1
	iret