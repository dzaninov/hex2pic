#define UART_ASM
#include "pic.inc"
#include "asm.inc"
#include "uart.inc"
#include "number.inc"

 global search_byte
 global checksum
 
                udata
search_byte     res         1               ; uart_find local
high_nibble     res         1               ; uart_get_hex local
hex_number      res         1               ; uart_send_hex local
checksum        res         1               ; updated in uart_get_hex
      
 code

; Get hex byte from UART to W and update checksum
; Z is set if checksum is 0
; locals: high_nibble

 routine uart_get_hex
        ; get high nibble

        lclcall uart_get                ; W = UART
        farcall hex_to_number           ; W = hex_to_number (W)
        relsel  high_nibble
        movwf   high_nibble             ; high_nibble = W
        swapf   high_nibble, f          ; high_nibble <<= 4

        ; get low nibble

        lclcall uart_get                ; W = UART
        farcall hex_to_number           ; W = hex_to_number (W)
        relsel  high_nibble
        iorwf   high_nibble, w          ; W |= high_nibble

        addwf   checksum, f             ; checksum += W
        return
 
; Send W to UART as hex
; locals: hex_number

 routine uart_send_hex
        relsel  hex_number
        movwf   hex_number
        swapf   hex_number, W           ; swap high and low nibble
        farcall number_to_hex           ; W = number_to_hex (hex_number)
        lclcall uart_send               ; send high nibble to UART

        relsel  hex_number
        movfw   hex_number              ; W = hex_number
        farcall number_to_hex           ; W = number_to_hex (hex_number)
        lclcall uart_send               ; send low nibble to UART
        return

; Send W to UART

 routine uart_send
        select  PIR1
        btfss   PIR1, TXIF              ; UART buffer is full ?
        repeat
        
        select  TXREG
        movwf   TXREG                   ; send
        return

; Get byte from UART to W

 routine uart_get
        select  RCSTA
        btfss   RCSTA, OERR             ; overrun error ?
        goto    no_oerr
        send    'o'
        lclcall uart_clear_errors
        reboot
        
no_oerr:
        mybank  RCSTA
        btfss   RCSTA, FERR             ; framing error ?
        goto    no_ferr
        send    'f'
        lclcall uart_clear_errors
        reboot

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
