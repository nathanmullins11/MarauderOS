bits 32
global serial_isr

extern serial_interrupt

serial_isr:
    cli
    call serial_interrupt
    sti
    iret
