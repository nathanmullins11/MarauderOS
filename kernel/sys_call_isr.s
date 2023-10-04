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

	push EAX
	push ECX
	push EDX
	push EBX
	push ESP
	push EBP
	push ESI
	push EDI

	push ESP

	; call function
	call sys_call

	; set stack pointer based on return value 
	mov ESP, EAX

	pop ESP
	pop EDI
	pop ESI
	pop EBP
	pop EBX
	pop EDX
	pop ECX
	pop EAX

	pop DS
	pop ES
	pop FS
	pop GS
	pop SS

	iret

	jmp end_if

	end_if:
	mov EAX, -1
	iret