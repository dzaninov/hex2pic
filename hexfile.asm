#define HEXFILE_ASM
#include "hexfile.inc"

 global record_type
 global low_address
 global high_address
 global record_buffer
 global bytes_to_read
 global words_to_write
    
                udata
record_type     res         1           ; set in read_record
low_address     res         1           ; low part of write address
high_address    res         1           ; high part of write address
record_buffer   res         MAX_RECORD  ; filled in queue_read_hex_data
bytes_to_read   res         1           ; used by queue_read_hex_data
words_to_write  res         1           ; used by write_data

    end
   