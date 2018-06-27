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

#endif
