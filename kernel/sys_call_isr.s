bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call            ; The C function that sys_call_isr will call
sys_call_isr:
	; push all segment registers
	push SS
	push GS
	push FS
	push ES
	push DS
	push CS

	; push all general purpose registers
	push EBP
	push EDI
	push ESI
	push EDX
	push ECX
	push EBX
	push EAX
	
	; push stack pointer
	push ESP

	; call function
	call sys_call

	; set stack pointer based on return value 
	mov ESP, EAX

	; pop registers in order of struct
	pop EAX
	pop EBX
	pop ECX
	pop EDX
	pop ESI
	pop EDI
	pop EBP

	pop CS
	pop DS
	pop ES
	pop FS
	pop GS
	pop SS

	iret