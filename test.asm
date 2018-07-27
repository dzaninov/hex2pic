#define TEST_ASM
#include "test.inc"
#include "uart.inc"

 global test_data

                udata
test_data       res         1

 code

 ; send data

 routine send_test
        inline  uart_init
        clrf    test_data
send_next1:
        rselect test_data
        incf    test_data, f
        movfw   test_data
        farcall uart_send
        goto    send_next1
        return

; send hex numbers

 routine hex_test
        inline  uart_init
        clrf    test_data
send_next2:
        rselect test_data
        incf    test_data, f
        movfw   test_data
        farcall uart_send_hex
        send    ':'
        goto    send_next2
        return

; send back received data

 routine echo_test
        inline  uart_init
get_next:
        farcall uart_get
        farcall uart_send
        goto    get_next
        return

; push data to FIFO and retrieve it

 routine fifo_test
        inline  uart_init
loop:
        rselect test_data
        movlf   '0', test_data
add_more:
        movfw   test_data
        farcall fifo_add
        rselect test_data
        incf    test_data, f
        skpgt   test_data, '9'
        goto    add_more

        clrf    test_data
get_more:
        farcall fifo_get
        farcall uart_send
        sendnl
        incf    test_data, f
        skpgt   test_data, 9
        goto    get_more
        goto    loop
        return

 end
