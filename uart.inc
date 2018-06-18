#ifndef UART_INC
#define UART_INC

    variable BAUD_RATE      = 115000    ; UART baud rate in bps
    variable UART_BUF_SIZE  = 16        ; UART buffer size
    variable XON            = 17        ; start data flow
    variable XOFF           = 19        ; stop data flow

; Setup UART

init_uart macro
        select  BAUDCTL
        bsf     BAUDCTL, BRG16
       
        local   BRG = ((CLOCK / BAUD_RATE) / 64) - 1
        local   BRGH = BRG / 0x100
        local   BRGL = BRG % 0x100

        select  SPBRG
        movlf   BRGH, SPBRGH
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
        
        select  delay_counter
        movlf   0xFF, delay_counter     ; let remote side detect start
        inline  short_delay             ; of serial data sequence 
        endm

;
; Send letter to UART
;

send    macro   byte
        movlw   byte
        call    uart_send
        endm

;
; Send debug letter to UART
;

debug   macro   letter
#if letter != 0
        send    letter
#endif
        endm

;
; Read from UART until W is found
;
; locals: search_byte
;

uart_find macro
get_next:
        movwf   search_byte             ; search_byte = W
        call    uart_get                ; W = UART

        xorwf   search_byte, f          ; search_byte ^= W
        bnz     get_next                ; no match -> get next
        endm

#endif
