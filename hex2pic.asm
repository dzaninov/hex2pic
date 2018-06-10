    include "p16f887.inc"

    list b=4,n=0,st=OFF     ; b: tab spaces
                            ; n: lines per page
                            ; st: symbol table dump
    radix dec

;===============================================================================
; mpasm / gpasm compatibility
    
#ifndef __GPUTILS_VERSION_MAJOR
    variable BANKING_ERROR  = 302       ; mpasm
    variable __EEPROM_START = 0x2100
#else
    variable BANKING_ERROR  = 1206      ; gpasm
    assume 0
#endif

;===============================================================================
; Device configuration

    __config _CONFIG1, _INTOSCIO & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF & _LVP_OFF & _DEBUG_OFF

    __config _CONFIG2, _BOR40V & _WRT_OFF 

;===============================================================================
; Configurable features

    variable SHOW_PROMPT = 0, CKSUM_MATCH = 0, IGNORE_WRITE = 0, WORD_WRITE = 0
    variable CR_END = 0, LF_END = 0, WRITE_PROTECT = 0, EEPROM_ADRH = 0, NOT_SUPPORTED = 0
    variable SCOPE = 0

; comment out a variable to disable feature

    variable SHOW_PROMPT    = '>'                   ; show ready to receive prompt
    variable CKSUM_MATCH    = 'C'                   ; show checksum verified state
    variable IGNORE_WRITE   = 'I'                   ; show ignored writes to protected area
    variable WORD_WRITE     = 'W'                   ; show writes
    variable CR_END         = '\r'                  ; send CR at the end
    variable LF_END         = '\n'                  ; send LF at the end
    variable WRITE_PROTECT  = 0xFF                  ; write protect up to this location
    variable EEPROM_ADRH    = __EEPROM_START >> 8   ; enable EEPROM support
    variable NOT_SUPPORTED  = 'U'                   ; show and fail on unsupported records
    variable SCOPE          = 1                     ; isolate sent data for easier capture

; these can be changed but not disabled

    variable LAST_RECORD    = '!'                   ; succesfull status char
    variable CKSUM_ERROR    = 'N'                   ; checksum does not match status char
    variable WRITE_ERROR    = 'E'                   ; write failed status char
    variable MAX_RECORD     = 0x10                  ; max record size

;===============================================================================
; Macros

bank0   macro                           ; select bank 0
        clrf    STATUS                  ; Z = 1, C = 0
        errorlevel +BANKING_ERROR   
        endm

bank1   macro                           ; select bank 2
        bcf     STATUS, RP1             ; RP1 = 0
        bsf     STATUS, RP0             ; RP0 = 1
        errorlevel -BANKING_ERROR
        endm

bank2   macro                           ; select bank 2
        bsf     STATUS, RP1             ; RP1 = 1
        bcf     STATUS, RP0             ; RP0 = 0
        errorlevel -BANKING_ERROR
        endm

bank3   macro                           ; select bank 3
        bsf     STATUS, RP1             ; RP1 = 1
        bsf     STATUS, RP0             ; RP0 = 1
        errorlevel -BANKING_ERROR
        endm

repeat  macro                           ; repeat until done
        goto    $ - 1
        endm

movff   macro   source, target          ; assign source to target file
        movfw   source                  ; W = source
        movwf   target                  ; target = W
        endm

movlf   macro   literal, file           ; assign literal to file
        movlw   literal                 ; w = literal
        movwf   file                    ; file = W
        endm

shr     macro   file, destination       ; shift right
        bcf     STATUS, C               ; clear carry
        rrf     file, destination       ; rotate right
        endm

skpeq   macro   file, literal           ; compare file with literal
#if literal == 0
        tstf    file                    ; zero check
#else
        movfw   file                    ; W = file
        sublw   literal                 ; W = literal - W
#endif
        skpz                            ; Z = 1 ?
        endm

skplte  macro   file, literal           ; file <= literal ?
        movfw   file                    ; W = file
        sublw   literal                 ; W = literal - W
        skpnc                           ; C = 0 ?
        endm

skpgt   macro   file, literal           ; file > literal ?
        movfw   file                    ; W = file
        sublw   literal                 ; W = literal - W
        skpc                            ; C = 1 ?
        endm

;===============================================================================
; Data section

bank0_data      udata
record_buffer   res         MAX_RECORD      ; filled in uart_read_hex_data
record_type     res         1               ; set in read_record
checksum        res         1               ; updated in uart_get_hex
bytes_to_read   res         1               ; used by uart_read_hex_data
delay_counter   res         1               ; short_delay
inner_delay     res         1               ; inner delay counter
outer_delay     res         1               ; outer delay counter

; locals
counter         res         1               ; main
search_byte     res         1               ; uart_find
high_nibble     res         1               ; uart_get_hex
byte_to_send    res         1               ; uart_send
hex_number      res         1               ; uart_send_hex

shared_data     udata_shr
low_address     res         1               ; low part of write address
high_address    res         1               ; high part of write address
words_to_write  res         1               ; used by write_data

;===============================================================================
; Inline code

inline  macro   function                ; call inline function
        function
        endm

#define throw   goto                    ; throw exception

try     macro   function                ; call function that can throw
        inline  function
        endm

catch   macro   exception               ; catch exception
exception:
        endm

;
; Short delay
;

short_delay macro
        decfsz  delay_counter, f
        repeat
        endm

;
; Setup PIC
;

pic_init macro
        bank3
        bsf     BAUDCTL, BRG16

        bank1
        movlf   B'01110000', OSCCON     ; switch to 8 Mhz clock

not_stable:
        btfss   OSCCON, HTS             ; clock stable ?
        goto    not_stable

        clrf    SPBRGH
        movlf   16, SPBRG               ; 115,200 at 8 Mhz
        movlf   B'00100100', TXSTA      ; BRGH: high speed
                                        ; TXEN: enable transmitter on RC6
        bank0
        movlf   B'10010000', RCSTA      ; SPEN: serial port enable
                                        ; CREN: enable receiver on RC7
        movlf   0xFF, delay_counter
        inline  short_delay             ; let remote side detect start
        endm

;
; Send letter to UART
;

send    macro   byte
        movlw   byte
        call    uart_send
        endm

;
; Send debug letter to UART
;

debug   macro   letter
#if letter != 0
        send    letter
#endif
        endm

;
; Read from UART until W is found
;
; locals: search_byte
;

uart_find macro
get_next:
        movwf   search_byte             ; search_byte = W
        call    uart_get                ; W = UART

        xorwf   search_byte, f          ; search_byte ^= W
        bnz     get_next                ; no match -> get next
        endm
    
;
; Read bytes_to_read bytes of data from UART to record_buffer
; and update checksum
;

uart_read_hex_data macro
        movlf   record_buffer, FSR      ; FSR = record_buffer

get_more_data:
        call    uart_get_hex            ; get data byte
        movwf   INDF                    ; *FSR = W
        call    uart_send_hex           ; uart_send_hex (W)
        send    ':'
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

;===============================================================================
; Code section
;

;
; Read hex file from UART and write it
;

 variable XON   = 17                    ; start data flow
 variable XOFF  = 19                    ; stop data flow

main    code    0
        inline  pic_init
;       debug   SHOW_PROMPT
        
loop:
        movfw   counter
        call    number_to_hex
        call    uart_send
        decf    counter, f
        goto    loop

next_record:
        inline  read_record
        send    XOFF

#if NOT_SUPPORTED != 0                  ; check for unsupported records ?
        try     process_record_safe
#else
        try     process_record_small
#endif
        send    XON
        goto    next_record

 catch  read_record_ERROR
 catch  process_record_EOF
 catch  write_word_ERROR                ; from process_record

        bank0
        call    uart_send               ; send final status from W

        debug   CR_END
        debug   LF_END

        goto    0                       ; restart

;
; Delay
;

delay
        movff   inner_delay, delay_counter
        inline  short_delay
        decfsz  outer_delay, f
        goto    delay
        return

;
; Get hex byte from UART to W and update checksum
; Z is set if checksum is 0
; locals: high_nibble
;

uart_get_hex
        ; get high nibble

        call    uart_get                ; W = UART
        call    hex_to_number           ; W = hex_to_number (W)
        movwf   high_nibble             ; high_nibble = W
        swapf   high_nibble, f          ; high_nibble <<= 4

        ; get low nibble

        call    uart_get                ; W = UART
        call    hex_to_number           ; W = hex_to_number (W)
        iorwf   high_nibble, f          ; W |= high_nibble

        addwf   checksum, f             ; checksum += W
        return
 
;
; Send W to UART as hex
; locals: hex_number
;

uart_send_hex
        movwf   hex_number
        swapf   hex_number, W           ; swap high and low nibble
        call    number_to_hex           ; W = number_to_hex (hex_number)
        call    uart_send               ; send high nibble to UART

        movfw   hex_number              ; W = hex_number
        call    number_to_hex           ; W = number_to_hex (hex_number)
        call    uart_send               ; send low nibble to UART
        return

;
; Send W to UART
; locals: byte_to_send
;

uart_send
        movwf   byte_to_send

not_ready:
        btfsc   PIR1, TXIF              ; UART buffer is full ?
        goto    ready_to_send           ; not full

        call    uart_clear_errors
        goto    not_ready

ready_to_send:
        movff   byte_to_send, TXREG     ; send

wait_for_send:
        btfsc   PIR1, TXIF              ; UART buffer is full ?
        goto    data_sent               ; not full

        call    uart_clear_errors
        goto    wait_for_send

data_sent:
#ifdef  SCOPE
        movlf   100, delay_counter
        inline  short_delay             ; isolate sent data
#endif
        return

;
; Get byte from UART to W
;

uart_get
        btfsc   PIR1, RCIF              ; UART buffer is empty ?
        goto    get_data                ; not empty

        call    uart_clear_errors
        goto    uart_get

get_data:
        movfw   RCREG                   ; return data to caller
        return

;
; Clear UART errors
;

uart_clear_errors
        btfss   RCSTA, OERR             ; overrun error ?
        goto    no_overrun

        bcf     RCSTA, CREN             ; disable receiver
        movfw   RCREG                   ; flush FIFO
        movfw   RCREG
        movfw   RCREG
        bsf     RCSTA, CREN             ; enable receiver

no_overrun:
        btfsc   RCSTA, FERR             ; no framing error ?
        movfw   RCREG                   ; clear framing error
        return

;
; Convert uppercase hex in W to number
;

hex_to_number
        sublw   'A'                     ; W -= 'A'
        skpnc
        addlw   7                       ; number: add 7 + 10
        addlw   10                      ; letter: add 10
        return

;
; Convert lower nibble of hex_number to uppercase hex
;

number_to_hex
        andlw   0x0F                    ; clear high nibble
        sublw   10                      ; W = 10 - W
        skpc
        addlw   'A' - '0'               ; letter
        addlw   '0'                     ; number
        return

;===============================================================================
; End of the program

        end

