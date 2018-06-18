#define NUMBER_ASM
#include "number.inc"

; Convert uppercase hex in W to number

hex_to_number
        addlw   0 - 'A'
        skpc
        addlw   'A' - '0' - 10          ; number
        addlw   10                      ; letter
        return

; Convert lower nibble of hex_number to uppercase hex

number_to_hex
        andlw   0x0F                    ; clear high nibble
        addlw   0 - 10                  ; W = W + (0 - 10)
        skpnc                           ; W <= 0xFF ?
        addlw   'A' - '0' - 10          ; letter
        addlw   '0' + 10                ; number
        return
