#ifndef CLOCK_INC
#define CLOCK_INC

; Set internal oscilator frequency and use it

set_intosc macro
#ifdef __16F887
        local   not_stable
      
#if CLOCK == 0
        error "Unknown clock frequency"
#endif
        select  OSCCON
        
#if CLOCK == 8000000
        movlf   B'01110001', OSCCON
#endif
#if CLOCK == 4000000
        movlf   B'01100001', OSCCON
#endif
#if CLOCK == 2000000
        movlf   B'01010001', OSCCON
#endif
#if CLOCK == 1000000
        movlf   B'01000001', OSCCON
#endif
#if CLOCK == 500000
        movlf   B'00110001', OSCCON
#endif
#if CLOCK == 250000
        movlf   B'00100001', OSCCON
#endif
#if CLOCK == 125000
        movlf   B'00010001', OSCCON
#endif
#if CLOCK == 31000
#define LFOSC
        movlf   B'00000001', OSCCON
#endif

not_stable:
#ifdef LFOSC
        btfss   OSCCON, LTS             ; low frequency clock stable ?
#else
        btfss   OSCCON, HTS             ; high frequency clock stable ?
#endif
        goto    not_stable
#endif        
        endm

#endif
