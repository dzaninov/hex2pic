#ifndef UART_INC
#define UART_INC

#include "delay.inc"
#include "queue.inc"

#ifndef UART_ASM
    extern uart_send_hex
    extern uart_send
    extern uart_get
    extern uart_clear_errors
#endif

#define BAUD_RATE   115000              ; UART baud rate in bps
#define XON         17                  ; start data flow
#define XOFF        19                  ; stop data flow

; Setup UART

init_uart macro
        local   BRG = ((CLOCK / BAUD_RATE) / BAUD_DIVIDER) - 1

#if BRG > MAX_WORD
        error "BRG overflow, over 16-bit"
#endif
#ifdef NO_BRG16
    #if BRG > MAX_BYTE
        error "BRG overflow, over 8-bit"
    #endif
#else
        inline  init_queue              ; init receive queue

        select  BAUDCTL
        bsf     BAUDCTL, BRG16
        
        select  SPBRGH
        movlf   HIGH_BYTE (BRG), SPBRGH
#endif
        select  SPBRG
        movlf   LOW_BYTE (BRG), SPBRG
        
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

; Receive data from UART and queue it

uart_queue macro
        local   check_uart
        local   no_data

check_uart:
        select  PIR1
        btfsc   PIR1, RCIF              ; UART buffer has data ?
        goto    no_data

        farcall uart_get
        inline  enqueue
        goto    check_uart              ; check if there is more data
no_data:
        endm

#endif
