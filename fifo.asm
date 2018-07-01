#define FIFO_ASM
#include "fifo.inc"
#include "interrupt.inc"
#include "number.inc"
#include "util.inc"
#include "uart.inc"

 global fifo_buffer
 global fifo_start
 global fifo_free
 global fifo_size
 global fifo_data
 global search_byte
 global checksum

                udata
fifo_buffer     res         MAX_FIFO_SIZE   ; buffer
fifo_start      res         1               ; start of the buffer
fifo_free       res         1               ; last + 1
fifo_size       res         1               ; data entries count
fifo_data       res         1               ; temporary storage
search_byte     res         1               ; fifo_find local
checksum        res         1               ; updated in fifo_get_hex
high_nibble     res         1               ; fifo_get_hex local

 code

 ; Queue data from W to FIFO

 routine fifo_add
        local   no_overflow

        rselect fifo_data
        movwf   fifo_data               ; fifo_data = W

        incf    fifo_size, f            ; fifo_size++
        movlw   MAX_FIFO_SIZE
        andwf   fifo_size, f            ; fifo_size &= MAX_FIFO_SIZE
        bnz     no_overflow             ; fifo_size != 0 ?
        reboot

no_overflow:
        rselecti fifo_buffer
        rmovlf  fifo_buffer, FSR        ; FSR = fifo_buffer
        movfw   fifo_free
        addwf   FSR, f                  ; FSR += fifo_free
        movff   fifo_data, INDF         ; *FSR = fifo_data

        incf    fifo_free, f            ; fifo_free++
        movlw   MAX_FIFO_SIZE
        andwf   fifo_free, f            ; rollover fifo_free

        fifo_debug 'i'
        return

 ; Get data from FIFO to W

 routine fifo_get
        local   no_data

        rselect fifo_size
no_data:
        tstf    fifo_size               ; fifo_size == 0 ?
        bz      no_data                 ; fifo_size == 0

        inline  disable_int             ; Disable interrupts
        decf    fifo_size, f            ; fifo_size--

        rselecti fifo_buffer
        rmovlf  fifo_buffer, FSR        ; FSR = fifo_buffer
        movfw   fifo_start
        addwf   FSR, f                  ; FSR += fifo_start
        movff   INDF, fifo_data         ; fifo_data = *FSR

        incf    fifo_start, f           ; fifo_start++
        movlw   MAX_FIFO_SIZE
        andwf   fifo_start, f           ; rollover fifo_start

        fifo_debug 'o'

        movfw   fifo_data
#if UART_INT == 1
        bsf     INTCON, GIE             ; enable interrupts
#endif
        return

; Get hex byte from FIFO to W and update checksum
; Z is set if checksum is 0
; locals: high_nibble

 routine fifo_get_hex
        ; get high nibble

        lclcall fifo_get
        farcall hex_to_number           ; W = hex_to_number (W)
        rselect high_nibble
        movwf   high_nibble             ; high_nibble = W
        swapf   high_nibble, f          ; high_nibble <<= 4

        ; get low nibble

        lclcall fifo_get
        farcall hex_to_number           ; W = hex_to_number (W)
        rselect high_nibble
        iorwf   high_nibble, w          ; W |= high_nibble

        addwf   checksum, f             ; checksum += W
        return

 end
