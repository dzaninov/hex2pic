#include "asm.inc"
    
 variable INT_CLOCK = 0, CLOCK = 0, SHOW_PROMPT = 0, CKSUM_MATCH = 0
 variable IGNORE_WRITE = 0, WORD_WRITE = 0, CR_END = 0, LF_END = 0
 variable WRITE_PROTECT = 0, EEPROM_ADRH = 0, NOT_SUPPORTED = 0

; comment out a variable to disable feature

 variable INT_CLOCK      = MAX_INT_CLOCK    ; use internal clock
 variable EEPROM_ADRH    = __EEPROM_ADRH    ; enable EEPROM support
 variable WRITE_PROTECT  = 0xFF             ; write protect up to this location
 variable SHOW_PROMPT    = '>'              ; show ready to receive prompt
 variable CKSUM_MATCH    = 'C'              ; show checksum verified state
 variable IGNORE_WRITE   = 'I'              ; show ignored writes to protected
 variable WORD_WRITE     = 'W'              ; show writes
 variable NOT_SUPPORTED  = 'U'              ; show and fail unsupported records
 variable CR_END         = '\r'             ; send CR at the end
 variable LF_END         = '\n'             ; send LF at the end

; these can be changed but not disabled

 variable CLOCK          = INT_CLOCK    ; change if not using internal clock
 variable LAST_RECORD    = '!'          ; succesfull status char
 variable CKSUM_ERROR    = 'N'          ; checksum does not match status char
 variable WRITE_ERROR    = 'E'          ; write failed status char
    
;===============================================================================
; Library includes

#include "interrupt.inc"
#include "clock.inc"
#include "uart.inc"
#include "hexfile.inc"

;===============================================================================
; Data section

            udata
counter     res     1                   ; main local
         
;===============================================================================
; Power on reset and reboot

 code                                   
        org 0
        setpage 0
        goto    main
        
;===============================================================================
; Interrupt service request
        
        org 4
        inline  int_start
        inline  uart_queue
        inline  int_end
        retfie

;===============================================================================
; Read hex file from UART and write it

main:
        unbank
        inline  set_intosc
        inline  init_uart
        debug   SHOW_PROMPT

next_record:
        send    XON
        inline  read_record
        send    XOFF

        try     process_record
        goto    next_record

 catch  read_record_ERROR
 catch  process_record_EOF
 catch  write_word_ERROR                ; from process_record

        farcall uart_send               ; send final status from W
        debug   CR_END
        debug   LF_END
        reboot
        
        end
