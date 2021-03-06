#include "config.inc"
#include "interrupt.inc"
#include "uart.inc"
#include "util.inc"
#include "clock.inc"
#include "test.inc"

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

;       farcall send_test
;       farcall hex_test
;       farcall echo_test
        farcall fifo_test

;       inline  boot_loader
        reboot

 end
