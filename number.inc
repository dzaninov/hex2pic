#ifndef NUMBER_INC
#define NUMBER_INC
    
#ifndef NUMBER_ASM
    extern hex_to_number                ; W = hex_to_number (W)
    extern number_to_hex                ; W = number_to_hex (W)
#endif

    variable kHZ = 1000
    variable MHZ = 1000 * kHZ

#endif
