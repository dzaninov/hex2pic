#ifndef QUEUE_INC
#define QUEUE_INC

#include "config.inc"

#ifndef QUEUE_ASM
    extern dequeue
    extern queue_get_hex
    extern queue_buffer
    extern queue_start
    extern queue_free
    extern queue_size
    extern queue_data
    extern search_byte
    extern checksum
#endif

#define MAX_QUEUE_SIZE  0xF             ; Must be X^2 - 1

; Initialize queue

init_queue macro
        clrf    queue_start
        clrf    queue_free
        clrf    queue_size
        endm

; Queue data from W

enqueue macro
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
        endm

; Read from queue until W is found
; locals: search_byte

queue_find macro
        local   get_next
        
        rselect search_byte
get_next:
        movwf   search_byte             ; search_byte = W
        farcall dequeue                 ; get data from queue

        rselect search_byte
        xorwf   search_byte, f          ; search_byte ^= W
        bnz     get_next                ; no match -> get next
        endm

#endif
