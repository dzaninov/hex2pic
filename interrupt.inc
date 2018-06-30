#ifndef INTERRUPT_INC
#define INTERRUPT_INC

#include "pic.inc"

#ifndef INTERRUPT_ASM
    #ifndef HAVE_SHADOW
        extern WREG_SHAD
        extern STATUS_SHAD
        extern PCLATH_SHAD
        extern FSR_SHAD
    #endif
#endif

; Save registers
; Reference: DS31008A, Example 8-1

context_save macro
#ifndef HAVE_SHADOW
        movwf   WREG_SHAD               ; Copy W to a Temporary Register
                                        ; regardless of current bank
        swapf   STATUS, w               ; Swap STATUS nibbles and place
                                        ; into W register
        mybank  0                       ; We are now in Bank0
        rselect STATUS_SHAD
        movwf   STATUS_SHAD             ; Save STATUS to a Temporary register
#ifndef ONE_PAGE
        movff   PCLATH, PCLATH_SHAD
#endif
        movff   FSR, FSR_SHAD
#endif
        endm

; Restore registers
; Reference: DS31008A, Example 8-1

context_restore macro
#ifndef HAVE_SHADOW
        rselect FSR_SHAD
        movff   FSR_SHAD, FSR
#ifndef ONE_PAGE
        movff   PCLATH_SHAD, PCLATH
#endif
        swapf   STATUS_SHAD, w      ; Swap original STATUS register value
                                    ; into W (restores original bank)
        movwf   STATUS              ; Restore STATUS register from
                                    ; W register
        swapf   WREG_SHAD, f        ; Swap WREG_SHAD nibbles and return
                                    ; value to WREG_SHAD
        swapf   WREG_SHAD, w        ; Swap WREG_SHAD to W to restore original
                                    ; W value without affecting STATUS 
#endif
        endm

; Disable interrupts
; Reference: AN576

disable_int macro
        local   int_enabled
        
int_enabled:
        bcf     INTCON, GIE             ; disable interrupts
        btfsc   INTCON, GIE             ; interrupts disabled ?
        goto    int_enabled
        endm

; start interrupt processing

int_start macro
        inline  disable_int
        inline  context_save
        endm

; end interrupt processing

int_end macro
        inline  context_restore
        endm
        
#endif
