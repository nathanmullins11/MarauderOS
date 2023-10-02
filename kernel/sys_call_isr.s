bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call            ; The C function that sys_call_isr will call
sys_call_isr:
	cmp eax, 1 ;; eax is IDLE
	je equal_to_one
	cmp eax, 0 ;; eax is EXIT
	je equal_to_zero

	equal_to_one:
	call sys_call
	jmp end_if

	equal_to_zero:
	call sys_call
	jmp end_if


	end_if: 
    mov eax, -1       ;; DELETE THIS LINE WHEN YOU IMPLEMENT R3
	;;; call sys_call
    iret