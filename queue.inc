#ifndef QUEUE_INC
#define QUEUE_INC

; Initialize queue

init_queue macro
        clrf    Q_START
        clrf    Q_FREE
        clrf    Q_SIZE
        endm

; Queue data from W

enqueue macro
        local   no_overflow
        
        rselect Q_DATA
        movwf   Q_DATA              ; Q_DATA = W
        
        incf    Q_SIZE, f           ; Q_SIZE++
        movlw   Q_MAX_SIZE
        andwf   Q_SIZE, f           ; Q_SIZE &= Q_MAX_SIZE
        bnz     no_overflow         ; Q_SIZE != 0 ?
        reboot
        
no_overflow:
        rselecti Q_BUFFER
        rmovlf  Q_BUFFER, FSR       ; FSR = Q_BUFFER
        movfw   Q_FREE
        addwf   FSR, f              ; FSR += Q_FREE
        movff   Q_DATA, INDF        ; *FSR = Q_DATA
        
        incf    Q_FREE, f           ; Q_FREE++
        movlw   Q_MAX_SIZE
        andwf   Q_FREE, f           ; Q_FREE &= Q_MAX_SIZE, rollover Q_FREE
        endm

; Dequeue into W

dequeue macro
        local   no_data
        inline  disable_int         ; Disable interrupts
        
        rselect Q_SIZE
no_data:
        testf   Q_SIZE              ; Q_SIZE == 0 ?
        bz      no_data             ; Q_SIZE == 0
        decf    Q_SIZE, f           ; Q_SIZE--
        
        rselecti Q_BUFFER
        rmovlf  Q_BUFFER, FSR       ; FSR = Q_BUFFER
        movfw   Q_START
        addwf   FSR, f              ; FSR += Q_START
        movff   INDF, Q_DATA        ; Q_DATA = *FSR
        
        incf    Q_START, f          ; Q_START++
        movlw   Q_MAX_SIZE
        andwf   Q_START, f          ; Q_START &= Q_MAX_SIZE, rollover Q_START
        
        movfw   Q_DATA
        bsf     INTCON, GIE         ; Enable interrupts
        endm

#endif
