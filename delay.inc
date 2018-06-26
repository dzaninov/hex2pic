#ifndef DELAY_INC
#define DELAY_INC

#ifndef DELAY_ASM
    extern long_delay           ; long_delay (inner_delay, outer_delay)
    extern inner_delay
    extern outer_delay
    extern delay_counter        ; short_delay (delay_counter)
#endif

; Short delay

short_delay macro
;       relsel  delay_counter       ; assumed
        decfsz  delay_counter, f
        repeat
        endm

#endif
