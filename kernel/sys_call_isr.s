bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call			; The C function that sys_call_isr will call
sys_call_isr:
	pusha ;; push all general purpose registers onto the stack. 

	;;; push remaining segment registers onto stack in reverse order of struck
	push GS 
	push FS
	push ES
	push DS
	push CS 

	push ESP ;; push ESP-pointer to a manually created context struct

	;;; sys_call() ;; call C function here

	;;; set ESP based on return value of your function (EAX)

	;;; pop registers in order of struct

	popa ;; pop all general purpose registers from the stack

	pop CS ;; pop segment registers
	pop DS
	pop ES
	pop FS
	pop GS

	;;; pop EFLAGS ;; pop flags register
	

	iret ;; return from ISR


