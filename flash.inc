#ifndef FLASH_INC
#define FLASH_INC

;
; Write one word
; switches to bank 3
; throws write_word_ERROR
;

write_word macro
#ifdef EEPROM_ADRH
        skpeq   high_address, EEPROM_ADRH ; EPPROM address ?
        goto    not_eeprom
        bcf     EECON1, EEPGD           ; setup writing to EEPROM
        goto    load_data
#endif

not_eeprom:
        bsf     EECON1, EEPGD           ; setup writing to program memory

load_data:
        movlf   0x55, EECON2            ; magic
        movlf   0xaa, EECON2            ; magic
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

;
; Write words_to_write words of data from record_buffer
; throws write_word_ERROR
;

write_data macro
        movlf   record_buffer, FSR      ; FSR = record_buffer

write_next_word:
        bank2

        movff   low_address, EEADR      ; EEADR = low_address
        movff   high_address, EEADRH    ; EEADRH = high_address

        incf    low_address, f          ; low_address++
        skpnc                           ; no low_address overflow ?
        incf    high_address, f         ; low_address overflow

        movff   INDF, EEDATA            ; low byte = *FSR
        incf    FSR, f                  ; FSR++

        movff   INDF, EEDATH            ; high byte = *FSR
        incf    FSR, f                  ; FSR++

        bsf     STATUS, RP0             ; switch bank 2 to 3
        try     write_word

        bank0

        debug   WORD_WRITE
        decfsz  words_to_write, f       ; --words_to_write == 0 ?
        goto    write_next_word         ; words_to_write != 0

        endm

#endif
