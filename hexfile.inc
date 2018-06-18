#ifndef HEXFILE_INC
#define HEXFILE_INC

    variable MAX_RECORD = 16            ; max hex data record size

;
; Read bytes_to_read bytes of data from UART to record_buffer
; and update checksum
;

uart_read_hex_data macro
        selecti record_buffer
        movlf   record_buffer, FSR      ; FSR = record_buffer

get_more_data:
        call    uart_get_hex            ; get data byte
        movwf   INDF                    ; *FSR = W
        incf    FSR, f                  ; FSR++
        decfsz  bytes_to_read, f        ; --bytes_to_read == 0 ?
        goto    get_more_data           ; bytes_to_read != 0
        endm

;
; Read record from UART
; throws read_record_ERROR
;

read_record macro
        movlw   ':'
        inline  uart_find               ; find start of the record

        call    uart_get_hex            ; get number of bytes in a record
        movwf   checksum                ; init checksum to record size
        movwf   bytes_to_read
        movwf   words_to_write
        shr     words_to_write, f       ; bytes to words

        call    uart_get_hex            ; get target high byte
        movwf   high_address

        call    uart_get_hex            ; get target low byte
        movwf   low_address

        call    uart_get_hex            ; get record type
        movwf   record_type

        inline  uart_read_hex_data      ; read whole record to record_buffer

        call    uart_get_hex            ; get record checksum
        bz      read_record_SUCCESS     ; checksum == 0 is good checksum

        movlw   CKSUM_ERROR
        throw   read_record_ERROR

read_record_SUCCESS:
        debug   CKSUM_MATCH
        endm

;
; Process data record
; throws write_word_ERROR
;

#if WRITE_PROTECT == 0
    variable IGNORE_WRITE = 0
#endif

process_data_record macro
#ifdef  WRITE_PROTECT
        tstf    high_address
        bz      write_record            ; high_address == 0
        
        skpgt   low_address, WRITE_PROTECT
        goto    ignore_write            ; ignore writes to protected area
#endif

write_record:
        try     write_data

#ifdef WRITE_PROTECT
        goto    process_data_record_SUCCESS

ignore_write:
        debug   IGNORE_WRITE
 
process_data_record_SUCCESS:
#endif
        endm

#if NOT_SUPPORTED != 0

;
; Process one hex file record
; check for unsupported records
; throws process_record_EOF and write_word_ERROR
;

 variable RECORD_DATA       = 0                 ; data record
 variable RECORD_EOF        = 1                 ; end of file record
 variable RECORD_HIGH_ADDR  = 4                 ; upper 16-bits of 32-bit address
 variable NO_ADDR_SUM       = 0xFA              ; checksum for RECORD_HIGH_ADDR with no address

process_record_safe macro
        skpeq   record_type, RECORD_DATA
        goto    not_data_record

        skplte  bytes_to_read, MAX_RECORD       ; bytes_to_read <= MAX_RECORD ?
        goto    not_supported

        try     process_data_record
        goto    process_record_MORE_DATA

not_data_record:
        skpeq   record_type, RECORD_EOF
        goto    not_eof_record

        movlw   LAST_RECORD
        throw   process_record_EOF

not_eof_record:
        skpeq   record_type, RECORD_HIGH_ADDR
        goto    not_supported                   ; no known record types match

        skpeq   checksum, NO_ADDR_SUM           ; process high address record
        goto    not_supported                   ; high address is set to non-zero

        goto    process_record_MORE_DATA        ; high address is set to zero, ignore

not_supported:
        movlw   NOT_SUPPORTED
        throw   process_record_EOF

process_record_MORE_DATA:
        endm

#else

;
; Same as above but don't check for unsupported records
; throws process_record_EOF and write_word_ERROR
;

 variable RECORD_EOF        = 0                 ; end of file record
 variable RECORD_HIGH_ADDR  = 2                 ; upper 16-bits of 32-bit address

process_record_small macro
        btfsc   record_type, RECORD_HIGH_ADDR   ; not high address record ?
        goto    process_record_MORE_DATA        ; ignore high address record

        btfsc   record_type, RECORD_EOF         ; not end of file record ?
        goto    data_record                     ; assume it is data record

        movlw   LAST_RECORD
        throw   process_record_EOF

data_record:
        try     process_data_record

process_record_MORE_DATA:
        endm
#endif

#endif
