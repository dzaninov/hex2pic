#ifndef TEST_INC
#define TEST_INC

#include "config.inc"

#ifndef TEST_ASM
    extern test_data
#endif

; send data

send_test macro
        local   send_next
        inline  uart_init
        clrf    test_data
send_next:
        rselect test_data
        incf    test_data, f
        movfw   test_data
        farcall uart_send
        goto    send_next
        endm

; send hex numbers

hex_test macro
        local   send_next
        inline  uart_init
        clrf    test_data
send_next:
        rselect test_data
        incf    test_data, f
        movfw   test_data
        farcall uart_send_hex
        send    ':'
        goto    send_next
        endm

; send back received data

echo_test macro
        local   get_next
        inline  uart_init
get_next:
        farcall uart_get
        farcall uart_send
        goto    get_next
        endm

; push data to queue and retrieve it

queue_test macro
        local   loop
        inline  uart_init
loop:
        movlw   '1'
        farcall enqueue
        movlw   '2'
        farcall enqueue
        
        farcall dequeue
        farcall uart_send
        farcall dequeue
        farcall uart_send
        goto    loop
        endm

#endif
