bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call            ; The C function that sys_call_isr will call
sys_call_isr:
	cmp EAX, 1
	je equal_to_one

	jmp end_if


	equal_to_one: 
	cli ; disablew interrupts
	; push reverse order of struct
	push EAX
	push ECX
	push EDX
	push EBX
	push EBP
	push ESI
	push EDI
	push SS
	push DS
	push ES
	push FS
	push GS
	push ESP
	
	; push stack pointer
	;; push ESP

	; call function
	call sys_call

	; set stack pointer based on return value 
	mov ESP, EAX

	pop ESP
	pop GS
	pop FS
	pop ES
	pop DS
	pop SS

	pop EDI
	pop ESI
	pop EBP
	pop EBX
	pop EDX
	pop ECX
	pop EAX

	; mov EAX, 0
	sti ; reenable interrupts
	iret
	jmp end_if

	end_if:
	mov EAX, -1
	iret