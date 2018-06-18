#ifndef INTERRUPT_INC
#define INTERRUPT_INC

; Save registers
; Reference: DS31008A, Example 8-1

context_save macro
        movwf   WREG_SHAD               ; Copy W to a Temporary Register
                                        ; regardless of current bank
        swapf   STATUS, w               ; Swap STATUS nibbles and place
                                        ; into W register
        mybank  0                       ; We are now in Bank0
        movwf   STATUS_SHAD             ; Save STATUS to a Temporary register
                                        ; in Bank0
        endm

; Restore registers
; Reference: DS31008A, Example 8-1

context_restore macro
        swapf   STATUS_SHAD, w          ; Swap original STATUS register value
                                        ; into W (restores original bank)
        movwf   STATUS                  ; Restore STATUS register from
                                        ; W register
        swapf   WREG_SHAD, f            ; Swap W_Temp nibbles and return
                                        ; value to W_Temp
        swapf   WREG_SHAD, w            ; Swap W_Temp to W to restore original
                                        ; W value without affecting STATUS 
        endm

; Disable interrupts
; Reference: AN576

disable_int macro
int_enabled:
        bcf     INTCON, GIE             ; disable interrupts
        btfsc   INTCON, GIE             ; interrupts disabled ?
        goto    int_enabled
        endm

#endif
