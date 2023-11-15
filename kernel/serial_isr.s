bits 32
global serial_isr

extern serial_init

serial_isr:
    cli
    call serial_init
    sti
    iret
