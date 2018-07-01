#ifndef UART_INC
#define UART_INC

#include "config.inc"
#include "number.inc"
#include "fifo.inc"
#include "delay.inc"

#ifndef UART_ASM
    extern uart_send_hex
    extern uart_send
    extern uart_get
    extern uart_clear_errors
#endif

#define BAUD_RATE       115000              ; UART baud rate in bps
#define XON             17                  ; start data flow
#define XOFF            19                  ; stop data flow
#define CR              '\r'                ; carriage return
#define LF              '\n'                ; line feed

#define uart_recv       fifo_get
#define uart_recv_hex   fifo_get_hex
#define uart_find       fifo_find

#ifdef NO_BRG16
    #define BAUD_DIVIDER    16
#else
    #define BAUD_DIVIDER    4
#endif

#ifndef CLOCK
    error "CLOCK not defined"
#endif

#define BRG     (CLOCK / (BAUD_RATE * BAUD_DIVIDER)) - 1

; Setup UART

uart_init macro
#if BRG > MAX_WORD
        error "BRG overflow, over 16-bit"
#endif
#ifdef NO_BRG16
    #if BRG > MAX_BYTE
        error "BRG overflow, over 8-bit"
    #endif
#else
        inline  init_fifo               ; init receive FIFO

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
#if UART_INT == 1
        select  PIE1
        bsf     PIE1, RCIE              ; Enable UART receive interrupt
        bsf     INTCON, GIE             ; Enable interrupts
#endif
        banksel delay_counter
        movlf   0xFF, delay_counter     ; let remote side detect start
        inline  short_delay             ; of serial data sequence
        endm

; Send literal to UART

send    macro   literal
        movlw   literal
        farcall uart_send
        endm

; Send newline to UART

sendnl  macro
        send    CR
        send    LF
        endm

; Send file to UART as hex

sendf   macro   file
        rselect file
        movfw   file
        farcall uart_send
        endm

; Send file to UART as hex

sendxf  macro   file
        rselect file
        movfw   file
        farcall uart_send_hex
        endm

; Send non-zero literal to UART

debug   macro   literal
#if literal != 0
        send    literal
#endif
        endm

; Conditionally send newline to UART

debugnl macro   send_it
#if send_it != 0
        sendnl
#endif
        endm

; Receive data from UART and add it to FIFO

uart_queue macro
        local   check_uart
        local   no_data

check_uart:
        select  PIR1
        btfsc   PIR1, RCIF              ; UART buffer has data ?
        goto    no_data

        farcall uart_get
        farcall fifo_add
        goto    check_uart              ; check if there is more data
no_data:
        endm

#endif
