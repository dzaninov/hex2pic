#ifndef LOADER_INC
#define LOADER_INC

#include "config.inc"
#include "uart.inc"

; configurable section

#define APP_START       0x200           ; start of loaded application
#define LOADER_PROMPT   '>'             ; boot loader prompt
#define LAST_RECORD     '!'             ; succesfull status char
#define CKSUM_ERROR     'N'             ; checksum does not match status char
#define WRITE_ERROR     'E'             ; write failed status char
#define IGNORED_WRITE   'I'             ; write to boot loader was ignored
#define NOT_SUPPORTED   'U'             ; unsupported hex file record

; comment out a variable to disable debug

 variable CKSUM_MATCH = 0, SHOW_WRITE = 0, NL_END = 0

 variable CKSUM_MATCH   = 'C'           ; show record checksum verified
 variable SHOW_WRITE    = 'W'           ; show each write
 variable NL_END        = 1             ; send newline at the end

; Read hex file from UART and write it

boot_loader macro
        uart_init
        send    LOADER_PROMPT

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
        debugnl NL_END
        endm

#endif
