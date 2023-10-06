bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call
sys_call_isr:
	;;; push reverse order of struct
	; push segment registers
	push SS
	push GS
	push FS
	push ES
	push DS
	
	; push gen purpose registers
	push EBP
	push EDI
	push ESI
	push EDX
	push ECX
	push EBX
	push EAX
	push ESP
	push ESP

	; call function
	call sys_call

	; set stack pointer based on return value 
	mov ESP, EAX

	; pop gen purpose registers
	pop ESP
	pop EAX
	pop EBX
	pop ECX
	pop EDX
	pop ESI
	pop EDI
	pop EBP

	; pop segment registers
	pop DS
	pop ES
	pop FS
	pop GS
	pop SS

	iret