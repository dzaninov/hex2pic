#ifndef QUEUE_INC
#define QUEUE_INC

#include "config.inc"

#ifndef QUEUE_ASM
    extern enqueue
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

#define DEBUG_QUEUE     1
#define MAX_QUEUE_SIZE  0xF             ; Must be X^2 - 1

; Initialize queue

init_queue macro
        rselect queue_start
        clrf    queue_start
        clrf    queue_free
        clrf    queue_size
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

queue_debug macro operation
#if DEBUG_QUEUE == 1
        send    '<'
        send    operation
        send    ':'
        sendxf  queue_size
        send    ':'
        sendf   queue_data
        send    ':'
        sendxf  queue_start
        send    ':'
        sendxf  queue_free
        send    '>'
        sendnl
#endif
        endm

#endif
