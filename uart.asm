#include "uart.inc"
#define UART_ASM
                
                data
uart_buffer     res         UART_BUF_SIZE   ; filled in UART interrupt handler
search_byte     res         1               ; uart_find local
high_nibble     res         1               ; uart_get_hex local
hex_number      res         1               ; uart_send_hex local
record_buffer   res         MAX_RECORD      ; filled in uart_read_hex_data
                code

;
; Get hex byte from UART to W and update checksum
; Z is set if checksum is 0
; locals: high_nibble
;

uart_get_hex
        ; get high nibble

        call    uart_get                ; W = UART
        call    hex_to_number           ; W = hex_to_number (W)
        movwf   high_nibble             ; high_nibble = W
        swapf   high_nibble, f          ; high_nibble <<= 4

        ; get low nibble

        call    uart_get                ; W = UART
        call    hex_to_number           ; W = hex_to_number (W)
        iorwf   high_nibble, w          ; W |= high_nibble

        addwf   checksum, f             ; checksum += W
        return
 
;
; Send W to UART as hex
; locals: hex_number
;

uart_send_hex
        movwf   hex_number
        swapf   hex_number, W           ; swap high and low nibble
        call    number_to_hex           ; W = number_to_hex (hex_number)
        call    uart_send               ; send high nibble to UART

        movfw   hex_number              ; W = hex_number
        call    number_to_hex           ; W = number_to_hex (hex_number)
        call    uart_send               ; send low nibble to UART
        return

;
; Send W to UART
;

uart_send
        btfss   PIR1, TXIF              ; UART buffer is full ?
        repeat
        
        movwf   TXREG                   ; send
        return

;
; Get byte from UART to W
;

uart_get
        btfss   RCSTA, OERR             ; overrun error ?
        goto    no_oerr
        send    'o'
        call    uart_clear_errors
        goto    0
        
no_oerr:
        btfss   RCSTA, FERR             ; framing error ?
        goto    no_ferr
        send    'f'
        call    uart_clear_errors
        goto    0

no_ferr:
        btfss   PIR1, RCIF              ; UART buffer is empty ?
        repeat                          ; not empty

        movfw   RCREG                   ; return data to caller
        return

;
; Clear UART errors
;

uart_clear_errors
        btfss   RCSTA, OERR             ; overrun error ?
        goto    no_overrun

        bcf     RCSTA, CREN             ; disable receiver
        movfw   RCREG                   ; flush FIFO
        movfw   RCREG
        movfw   RCREG
        bsf     RCSTA, CREN             ; enable receiver

no_overrun:
        btfsc   RCSTA, FERR             ; no framing error ?
        movfw   RCREG                   ; clear framing error
        return
