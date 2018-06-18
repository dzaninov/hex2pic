#define DELAY_ASM
#include "delay.inc"

                udata
inner_delay     res         1           ; long_delay parameter
outer_delay     res         1           ; long_delay parameter
delay_counter   res         1           ; short_delay parameter
                code

; Long delay
; Arguments: inner_delay, outer_delay

long_delay
        movff   inner_delay, delay_counter
        inline  short_delay
        decfsz  outer_delay, f
        goto    delay
        return
