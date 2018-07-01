#define UART_ASM
#include "uart.inc"

                udata
hex_number      res         1           ; uart_send_hex local

 code

; Send W to UART

 routine uart_send
        select  PIR1
        btfss   PIR1, TXIF              ; UART buffer is full ?
        repeat
        
        select  TXREG
        movwf   TXREG                   ; send
        return

; Send W to UART as hex
; locals: hex_number

 routine uart_send_hex
        rselect hex_number
        movwf   hex_number
        swapf   hex_number, W           ; swap high and low nibble
        farcall number_to_hex           ; W = number_to_hex (hex_number)
        lclcall uart_send               ; send high nibble to UART

        rselect hex_number
        movfw   hex_number              ; W = hex_number
        farcall number_to_hex           ; W = number_to_hex (hex_number)
        lclcall uart_send               ; send low nibble to UART
        return

; Get byte from UART to W

 routine uart_get
        select  RCSTA
        btfss   RCSTA, OERR             ; overrun error ?
        goto    no_oerr
        send    'o'
        lclcall uart_clear_errors
        
no_oerr:
        mybank  RCSTA
        btfss   RCSTA, FERR             ; framing error ?
        goto    no_ferr
        send    'f'
        lclcall uart_clear_errors

no_ferr:
        select  PIR1
        btfss   PIR1, RCIF              ; UART buffer is empty ?
        repeat                          ; not empty

        select  RCREG
        movfw   RCREG                   ; return data to caller
        return

; Clear UART errors

 routine uart_clear_errors
        select  RCSTA
        btfss   RCSTA, OERR             ; overrun error ?
        goto    no_overrun

        bcf     RCSTA, CREN             ; disable receiver
        select  RCREG
        movfw   RCREG                   ; flush FIFO
        movfw   RCREG
        movfw   RCREG
        select  RCSTA
        bsf     RCSTA, CREN             ; enable receiver

no_overrun:
        btfsc   RCSTA, FERR             ; no framing error ?
        select  RCREG
        movfw   RCREG                   ; clear framing error
        return

 end
