#ifndef PIC_INC
#define PIC_INC

#include "number.inc"

;===============================================================================
; PIC compatibility
; Shared: PCLATH, STATUS, FSR, INDF, INTCON

#ifdef __16F88
    #include "p16f88.inc"
    #define __16F88X
    variable BAUD_DIVIDER = 16
    #define NO_BRG16
#endif
    
#ifdef __16F887
    #include "p16f887.inc"
    #define __16F88X
    variable BAUD_DIVIDER = 64
#endif

#ifdef __16F88X
    variable MAX_INT_CLOCK = 8 * MHZ
#ifndef __EEPROM_START
    variable __EEPROM_START = 0x2100
#endif
#endif

#ifdef __16F18313
    #include "p16f18313.inc"
    #define __16F18XXX
    #define ONE_PAGE
#endif
    
#ifdef __16F18346
    #include "p16f18346.inc"
    #define __16F18XXX
#endif

#ifdef __16F18XXX
    variable MAX_INT_CLOCK = 32 * MHZ
    variable BAUD_DIVIDER = 64
#ifndef __EEPROM_START
    variable __EEPROM_START = 0xF000
#endif
    #define HAVE_SHADOW
    #define HAVE_FSRH
    #define FSR         FSR0L
    #define FSRH        FSR0H
    #define INDF        INDF0
    #define EADDR       NVMADRL
    #define EADDRH      NVMADRH
    #define EEDATA      NVMDATL
#endif

#ifndef MAX_INT_CLOCK
    error "Unknown processor"
#endif

    variable __EEPROM_ADRH  = __EEPROM_START >> 8
    variable BANK_SIZE      = 0x80

#endif
