#ifndef NUMBER_INC
#define NUMBER_INC

    variable kHZ = 1000
    variable MHZ = 1000 * kHZ

    mkexport NUMBER_ASM
    
    export hex_to_number        ; W = hex_to_number (W)
    export number_to_hex        ; W = number_to_hex (W)

#endif
