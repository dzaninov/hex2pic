#ifndef UART_INC
#define UART_INC

#include "delay.inc"

#ifndef UART_ASM
    extern uart_get_hex
    extern uart_send_hex
    extern uart_send
    extern uart_get
    extern uart_clear_errors
    extern search_byte
    extern checksum
#endif

    variable BAUD_RATE      = 115000    ; UART baud rate in bps
    variable UART_BUF_SIZE  = 16        ; UART buffer size
    variable XON            = 17        ; start data flow
    variable XOFF           = 19        ; stop data flow

; Setup UART

init_uart macro
        local   BRG = ((CLOCK / BAUD_RATE) / BAUD_DIVIDER) - 1
#if BRG > 0xFFFF
        error "BRG overflow, over 16-bit"
#endif
        
#ifdef NO_BRG16
#if BRG > 0xFF
        error "BRG overflow, over 8-bit"
#endif
#else
        select  BAUDCTL
        bsf     BAUDCTL, BRG16
        
        local   BRGH = BRG / 0x100
        select  SPBRGH
        movlf   BRGH, SPBRGH
#endif
        local   BRGL = BRG % 0x100
        select  SPBRG
        movlf   BRGL, SPBRG
        
        select  TXSTA
        movlf   B'00100100', TXSTA      ; BRGH: high speed
                                        ; TXEN: enable transmitter on RC6
        select  RCSTA
        movlf   B'10010000', RCSTA      ; SPEN: serial port enable
                                        ; CREN: enable receiver on RC7
        select  PIE1
        bsf     PIE1, RCIE              ; Enable UART receive interrupt
        bsf     INTCON, GIE             ; Enable interrupts
        
        banksel delay_counter
        movlf   0xFF, delay_counter     ; let remote side detect start
        inline  short_delay             ; of serial data sequence 
        endm

; Send letter to UART

send    macro   byte
        movlw   byte
        farcall uart_send
        endm

; Send debug letter to UART

debug   macro   letter
#if letter != 0
        send    letter
#endif
        endm

; Read from UART until W is found
; locals: search_byte

uart_find macro
        local   get_next
        
        rselect search_byte
get_next:
        movwf   search_byte             ; search_byte = W
        farcall uart_get                ; W = UART

        rselect search_byte
        xorwf   search_byte, f          ; search_byte ^= W
        bnz     get_next                ; no match -> get next
        endm

#endif
