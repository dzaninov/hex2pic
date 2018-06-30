#include "asm.inc"

#define CLOCK   MAX_INT_CLOCK           ; clock speed in Hz
#include "interrupt.inc"
#include "clock.inc"
#include "uart.inc"
#include "loader.inc"
#include "util.inc"

 code                                   
        org 0                           ; Power on reset and reboot
        setpage 0
        goto    main
        
        org 4                           ; Interrupt service request
        inline  int_start
        inline  uart_queue
        inline  int_end
        retfie

; program start

main:
        unbank
        inline  set_clock
        inline  boot_loader
        reboot
        
        end
