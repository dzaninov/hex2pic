#ifndef TEST_INC
#define TEST_INC

#include "uart.inc"

; UART echo test

echo_test macro
        local   get_next
        inline  uart_init
get_next:
        farcall uart_recv
        farcall uart_send
        goto    get_next
        endm

#endif
