#ifndef PIC_INC
#define PIC_INC

#define kHZ(x)  ((x) * 1000)
#define MHZ(x)  (kHZ ((x) * 1000))

;===============================================================================
; PIC compatibility
; Shared: PCLATH, STATUS, FSR, INDF, INTCON

#ifdef __16F88
    #include "p16f88.inc"
    #define __16F88X
    #define NO_BRG16
#endif

#ifdef __16F887
    #include "p16f887.inc"
    #define __16F88X
#endif

#ifdef __16F88X
    #define MAX_INT_CLOCK   (MHZ (8))
    #ifndef __EEPROM_START
        #define __EEPROM_START  0x2100
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
    #define MAX_INT_CLOCK   (MHZ (32))
    #ifndef __EEPROM_START
        #define __EEPROM_START  0xF000
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

#define __EEPROM_ADRH   (__EEPROM_START >> 8)
#define BANK_SIZE       0x80

#endif
