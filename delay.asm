#define DELAY_ASM
#include "delay.inc"

 global inner_delay
 global outer_delay
 global delay_counter

                udata
inner_delay     res         1           ; long_delay parameter
outer_delay     res         1           ; long_delay parameter
delay_counter   res         1           ; short_delay parameter

 code

; Long delay
; Arguments: inner_delay, outer_delay

 routine long_delay
 ;      rselect delay_counter           ; assumed
        movff   inner_delay, delay_counter
        inline  short_delay
        decfsz  outer_delay, f
        goto    long_delay
        return

 end
