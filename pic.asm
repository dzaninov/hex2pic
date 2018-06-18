;===============================================================================
; Vector table
;
        extern  main
        extern  isr
        
        code    0                       ; power on reset
        goto    main
        
        org     4                       ; interrupt
        goto    isr
        