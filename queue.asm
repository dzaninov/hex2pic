#define QUEUE_ASM
#include "queue.inc"
#include "interrupt.inc"
#include "number.inc"
#include "util.inc"
#include "uart.inc"

 global queue_buffer
 global queue_start
 global queue_free
 global queue_size
 global queue_data
 global search_byte
 global checksum

                udata
queue_buffer    res         MAX_QUEUE_SIZE      ; buffer
queue_start     res         1                   ; start of the buffer
queue_free      res         1                   ; last + 1
queue_size      res         1                   ; data entries count
queue_data      res         1                   ; temporary storage
search_byte     res         1                   ; queue_find local
checksum        res         1                   ; updated in queue_get_hex
high_nibble     res         1                   ; queue_get_hex local

 code

 ; Queue data from W

 routine enqueue
        local   no_overflow

        rselect queue_data
        movwf   queue_data              ; queue_data = W

        incf    queue_size, f           ; queue_size++
        movlw   MAX_QUEUE_SIZE
        andwf   queue_size, f           ; queue_size &= Q_MAX_SIZE
        bnz     no_overflow             ; queue_size != 0 ?
        reboot

no_overflow:
        rselecti queue_buffer
        rmovlf  queue_buffer, FSR       ; FSR = queue_buffer
        movfw   queue_free
        addwf   FSR, f                  ; FSR += queue_free
        movff   queue_data, INDF        ; *FSR = Q_DATA

        incf    queue_free, f           ; queue_free++
        movlw   MAX_QUEUE_SIZE
        andwf   queue_free, f           ; rollover queue_free

        queue_debug 'i'
        return

 ; Dequeue into W

 routine dequeue
        local   no_data

        rselect queue_size
no_data:
        tstf    queue_size              ; queue_size == 0 ?
        bz      no_data                 ; queue_size == 0

        inline  disable_int             ; Disable interrupts
        decf    queue_size, f           ; queue_size--

        rselecti queue_buffer
        rmovlf  queue_buffer, FSR       ; FSR = queue_buffer
        movfw   queue_start
        addwf   FSR, f                  ; FSR += queue_start
        movff   INDF, queue_data        ; queue_data = *FSR

        incf    queue_start, f          ; queue_start++
        movlw   MAX_QUEUE_SIZE
        andwf   queue_start, f          ; rollover queue_start

        queue_debug 'o'

        movfw   queue_data
#if UART_INT == 1
        bsf     INTCON, GIE             ; enable interrupts
#endif
        return

; Get hex byte from queue to W and update checksum
; Z is set if checksum is 0
; locals: high_nibble

 routine queue_get_hex
        ; get high nibble

        lclcall dequeue                 ; get data from queue
        farcall hex_to_number           ; W = hex_to_number (W)
        rselect high_nibble
        movwf   high_nibble             ; high_nibble = W
        swapf   high_nibble, f          ; high_nibble <<= 4

        ; get low nibble

        lclcall dequeue                 ; get data from queue
        farcall hex_to_number           ; W = hex_to_number (W)
        rselect high_nibble
        iorwf   high_nibble, w          ; W |= high_nibble

        addwf   checksum, f             ; checksum += W
        return

 end
