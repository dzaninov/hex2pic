#ifndef PIC_INC
#define PIC_INC

#include "number.inc"

;===============================================================================
; PIC compatibility
; Shared: PCLATH, STATUS, FSR, INDF, INTCON
; __EEPROM_START is builtin on gpasm

#ifdef __16F887
    #include "p16f887.inc"
    variable MAX_INT_CLOCK = 8 * MHZ
#ifndef __EEPROM_START
    variable __EEPROM_START = 0x2100
#endif
#endif
    
#ifdef __16F18313
    #include "p16f18313.inc"
    variable MAX_INT_CLOCK = 32 * MHZ
#ifndef __EEPROM_START
    variable __EEPROM_START = 0xF000
#endif
    #define HAVE_SHADOW
    #define HAVE_FSRH
    #define NO_PAGESEL
    #define FSR         FSR0L
    #define FSRH        FSR0H
    #define INDF        INDF0
;   #define BAUDCTL     BAUD1CON
;   #define SPBRG       SP1BRGL
;   #define SPBRGH      SP1BRGH
;   #define TXSTA       TX1STA
;   #define RCSTA       RC1STA
;   #define TXREG       TX1REG
;   #define RCREG       RC1REG
    #define EADDR       NVMADRL
    #define EADDRH      NVMADRH
    #define EEDATA      NVMDATL
;   #define EEDATH      NVMDATH
#endif

#ifndef MAX_INT_CLOCK
    error "Unknown processor"
#endif

    variable __EEPROM_ADRH  = __EEPROM_START >> 8
    variable BANK_SIZE      = 0x80

; Set internal oscilator frequency and use it

set_intosc macro
        local   not_stable
        
#if CLOCK == 0
        error "Unknown clock frequency"
#endif
        select  OSCCON
#if CLOCK == 8000000
        movlf   B'01110001', OSCCON
#elif CLOCK == 4000000
        movlf   B'01100001', OSCCON
#elif CLOCK == 2000000
        movlf   B'01010001', OSCCON
#elif CLOCK == 1000000
        movlf   B'01000001', OSCCON
#elif CLOCK == 500000
        movlf   B'00110001', OSCCON
#elif CLOCK == 250000
        movlf   B'00100001', OSCCON
#elif CLOCK == 250000
        movlf   B'00010001', OSCCON
#elif CLOCK == 31000
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
        endm
        
#endif
