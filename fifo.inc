#ifndef FIFO_INC
#define FIFO_INC

#include "config.inc"

#ifndef FIFO_ASM
    extern fifo_add
    extern fifo_get
    extern fifo_get_hex
    extern fifo_buffer
    extern fifo_start
    extern fifo_free
    extern fifo_size
    extern fifo_data
    extern search_byte
    extern checksum
#endif

#define DEBUG_FIFO      1
#define MAX_FIFO_SIZE   0xF             ; Must be X^2 - 1

; Initialize FIFO

init_fifo macro
        rselect fifo_start
        clrf    fifo_start
        clrf    fifo_free
        clrf    fifo_size
        endm

; Read from FIFO until W is found
; locals: search_byte

fifo_find macro
        local   get_next

        rselect search_byte
get_next:
        movwf   search_byte             ; search_byte = W
        farcall fifo_get

        rselect search_byte
        xorwf   search_byte, f          ; search_byte ^= W
        bnz     get_next                ; no match -> get next
        endm

fifo_debug macro operation
#if DEBUG_FIFO == 1
        send    '<'
        send    operation
        send    ':'
        sendxf  fifo_size
        send    ':'
        sendf   fifo_data
        send    ':'
        sendxf  fifo_start
        send    ':'
        sendxf  fifo_free
        send    '>'
        sendnl
#endif
        endm

#endif
