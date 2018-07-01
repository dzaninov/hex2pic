#ifndef HEXFILE_INC
#define HEXFILE_INC

#include "asm.inc"
#include "flash.inc"
#include "queue.inc"
    
#ifndef HEXFILE_ASM
    extern record_type
    extern low_address
    extern high_address
    extern record_buffer
    extern bytes_to_read
    extern words_to_write
#endif

#define MAX_RECORD  16                  ; max hex data record size

; Read bytes_to_read bytes of data from UART to record_buffer
; and update checksum

read_hex_data macro
        local   get_more_data
        
        bankisel record_buffer
        rmovlf  record_buffer, FSR      ; FSR = record_buffer

get_more_data:
        farcall uart_recv_hex           ; get data byte
        movwf   INDF                    ; *FSR = W
        incf    FSR, f                  ; FSR++
        rselect bytes_to_read
        decfsz  bytes_to_read, f        ; --bytes_to_read == 0 ?
        goto    get_more_data           ; bytes_to_read != 0
        endm

; Read record from queue
; throws read_record_ERROR

read_record macro
        local   read_record_SUCCESS
        
        movlw   ':'
        inline  uart_find               ; find start of the record

        farcall uart_recv_hex           ; get number of bytes in a record
        rselect checksum
        movwf   checksum                ; init checksum to record size
        rselect bytes_to_read
        movwf   bytes_to_read
        movwf   words_to_write
        shr     words_to_write, f       ; bytes to words

        farcall uart_recv_hex           ; get target high byte
        rselect high_address
        movwf   high_address

        farcall uart_recv_hex           ; get target low byte
        rselect low_address
        movwf   low_address

        farcall uart_recv_hex           ; get record type
        rselect record_type
        movwf   record_type

        inline  read_hex_data           ; read whole record to record_buffer

        farcall uart_recv_hex           ; get record checksum
        bz      read_record_SUCCESS     ; checksum == 0 is good checksum

        movlw   CKSUM_ERROR
        throw   read_record_ERROR

read_record_SUCCESS:
        debug   CKSUM_MATCH
        endm

; Process data record
; throws write_word_ERROR

process_data_record macro
        local   write_record
        local   process_data_record_SUCCESS
        local   ignore_write
        
        rselect high_address
        tstf    high_address
        bz      write_record            ; high_address == 0
        
        local   PROT_HIGH = HIGH_BYTE (APP_START - 1)
        local   PROT_LOW = LOW_BYTE (APP_START - 1)
 
        skpgt   high_address, PROT_HIGH
        goto    ignore_write            ; ignore writes to boot loader
        
        skpgt   low_address, PROT_LOW
        goto    ignore_write            ; ignore writes to boot loader

write_record:
        try     write_data
        goto    process_data_record_SUCCESS

ignore_write:
        send    IGNORED_WRITE
 
process_data_record_SUCCESS:
        endm

; Process one hex file record
; throws process_record_EOF and write_word_ERROR

#define RECORD_DATA         0           ; data record
#define RECORD_EOF          1           ; end of file record
#define RECORD_HIGH_ADDR    4           ; upper 16-bits of 32-bit address
#define NO_ADDR_SUM         0xFA        ; no address RECORD_HIGH_ADDR checksum

process_record macro
        local   not_data_record
        local   not_eof_record
        local   not_supported
        local   process_record_MORE_DATA
        
        rselect record_type
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

        rselect checksum
        skpeq   checksum, NO_ADDR_SUM      ; process high address record
        goto    not_supported              ; high address is set to non-zero

        goto    process_record_MORE_DATA   ; high address is set to zero, ignore

not_supported:
        movlw   NOT_SUPPORTED
        throw   process_record_EOF

process_record_MORE_DATA:
        endm

#endif
