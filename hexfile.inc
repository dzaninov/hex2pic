#ifndef HEXFILE_INC
#define HEXFILE_INC

#include "asm.inc"
#include "flash.inc"
    
#ifndef HEXFILE_ASM
    extern record_type
    extern low_address
    extern high_address
    extern record_buffer
    extern bytes_to_read
    extern words_to_write
#endif

 variable MAX_RECORD = 16               ; max hex data record size

; Read bytes_to_read bytes of data from UART to record_buffer
; and update checksum

uart_read_hex_data macro
        local   get_more_data
        
        bankisel record_buffer
        movlw   record_buffer
        movwf   FSR                     ; FSR = record_buffer

get_more_data:
        farcall uart_get_hex            ; get data byte
        movwf   INDF                    ; *FSR = W
        incf    FSR, f                  ; FSR++
        relsel  bytes_to_read
        decfsz  bytes_to_read, f        ; --bytes_to_read == 0 ?
        goto    get_more_data           ; bytes_to_read != 0
        endm

; Read record from UART
; throws read_record_ERROR

read_record macro
        local   read_record_SUCCESS
        
        movlw   ':'
        inline  uart_find               ; find start of the record

        farcall uart_get_hex            ; get number of bytes in a record
        relsel  checksum
        movwf   checksum                ; init checksum to record size
        relsel  bytes_to_read
        movwf   bytes_to_read
        movwf   words_to_write
        shr     words_to_write, f       ; bytes to words

        farcall uart_get_hex            ; get target high byte
        relsel  high_address
        movwf   high_address

        farcall uart_get_hex            ; get target low byte
        relsel  low_address
        movwf   low_address

        farcall uart_get_hex            ; get record type
        relsel  record_type
        movwf   record_type

        inline  uart_read_hex_data      ; read whole record to record_buffer

        farcall uart_get_hex            ; get record checksum
        bz      read_record_SUCCESS     ; checksum == 0 is good checksum

        movlw   CKSUM_ERROR
        throw   read_record_ERROR

read_record_SUCCESS:
        debug   CKSUM_MATCH
        endm

; Process data record
; throws write_word_ERROR

#ifndef WRITE_PROTECT
    variable WRITE_PROTECT = 0        
#endif

#if WRITE_PROTECT == 0
    variable IGNORE_WRITE = 0
#endif

process_data_record macro
        local   write_record
        local   process_data_record_SUCCESS
        
#if WRITE_PROTECT != 0
        local   ignore_write
        
        relsel  high_address
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

; Process one hex file record
; throws process_record_EOF and write_word_ERROR

 variable RECORD_DATA       = 0         ; data record
 variable RECORD_EOF        = 1         ; end of file record
 variable RECORD_HIGH_ADDR  = 4         ; upper 16-bits of 32-bit address
 variable NO_ADDR_SUM       = 0xFA      ; no address RECORD_HIGH_ADDR checksum

process_record macro
        local   not_data_record
        local   not_eof_record
        local   not_supported
        local   process_record_MORE_DATA
        
        relsel  record_type
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
        goto    not_supported              ; no known record types match

        relsel  checksum
        skpeq   checksum, NO_ADDR_SUM      ; process high address record
        goto    not_supported              ; high address is set to non-zero

        goto    process_record_MORE_DATA   ; high address is set to zero, ignore

not_supported:
        movlw   NOT_SUPPORTED
        throw   process_record_EOF

process_record_MORE_DATA:
        endm

#endif
