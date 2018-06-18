
                udata
record_type     res         1               ; set in read_record
checksum        res         1               ; updated in uart_get_hex
bytes_to_read   res         1               ; used by uart_read_hex_data
words_to_write  res         1               ; used by write_data
low_address     res         1               ; low part of write address
high_address    res         1               ; high part of write address
