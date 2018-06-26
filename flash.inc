#ifndef FLASH_INC
#define FLASH_INC

#include "hexfile.inc"

; Write one word
; throws write_word_ERROR

write_word macro
        local   not_eeprom
        local   load_data
        local   write_word_SUCCESS
        
        relsel  high_address
        skpeq   high_address, EEPROM_ADRH ; EPPROM address ?
        goto    not_eeprom
        
        select  EECON1
        bcf     EECON1, EEPGD           ; setup writing to EEPROM
        goto    load_data

not_eeprom:
        forcsel EECON1
        bsf     EECON1, EEPGD           ; setup writing to program memory

load_data:
        mybank  EECON1
        select  EECON2
        movlf   0x55, EECON2            ; magic
        movlf   0xaa, EECON2            ; magic
        select  EECON1
        bsf     EECON1, WR              ; write

        btfsc   EECON1, WR              ; wait until done
        repeat

        btfss   EECON1, WRERR           ; error is set ?
        goto    write_word_SUCCESS

        btfsc   EECON1, WRERR           ; clear error
        movlw   WRITE_ERROR
        throw   write_word_ERROR

write_word_SUCCESS:
        endm

; Write words_to_write words of data from record_buffer
; args: record_buffer, low_address, high_address, words_to_write
; throws write_word_ERROR

write_data macro
        local   write_next_word
        
        relsel  record_buffer
        movlf   record_buffer, FSR      ; FSR = record_buffer

write_next_word:
;       relsel  low_address             ; assumed
        movfw   low_address
        select  EEADR
        movwf   EEADR                   ; EEADR = low_address

;       relsel  high_address            ; assumed
        movfw   high_address
        select  EEADRH
        movwf   EEADRH                  ; EEADRH = high_address
        
        relsel  low_address
        incf    low_address, f          ; low_address++
        skpnc                           ; no low_address overflow ?
        incf    high_address, f         ; low_address overflow

        select  EEDATA
        movff   INDF, EEDATA            ; low byte = *FSR
        incf    FSR, f                  ; FSR++

        select  EEDATH
        movff   INDF, EEDATH            ; high byte = *FSR
        incf    FSR, f                  ; FSR++

        try     write_word

        debug   WORD_WRITE
        relsel  words_to_write
        decfsz  words_to_write, f       ; --words_to_write == 0 ?
        goto    write_next_word         ; words_to_write != 0

        endm

#endif