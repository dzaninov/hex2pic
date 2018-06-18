#ifndef DELAY_INC
#define DELAY_INC

    mkexport DELAY_ASM
    
    export long_delay           ; long_delay (inner_delay, outer_delay)
    export inner_delay
    export outer_delay
    
    export delay_counter        ; short_delay (delay_counter)

; Short delay

short_delay macro
        decfsz  delay_counter, f
        repeat
        endm

#endif
