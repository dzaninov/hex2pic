#ifndef NUMBER_INC
#define NUMBER_INC
        
#include "config.inc"

#ifndef NUMBER_ASM
    extern hex_to_number                ; W = hex_to_number (W)
    extern number_to_hex                ; W = number_to_hex (W)
#endif

#define MAX_BYTE        0xFF
#define MAX_WORD        0xFFFF
#define LOW_BYTE(x)     ((x) & 0xFF)
#define HIGH_BYTE(x)    ((x) >> 8)
    
#endif
