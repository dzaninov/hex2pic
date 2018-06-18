#ifndef ASSEMBLER_INC
#define ASSEMBLER_INC

    radix dec
    list b=4,n=0,st=OFF                 ; b: tab spaces
                                        ; n: lines per page
                                        ; st: symbol table dump

#ifndef __GPUTILS_VERSION_MAJOR
    variable BANKING_ERROR = 1206       ; gpasm
#else
    variable BANKING_ERROR = 302        ; mpasm
#endif

mkexport macro source                   ; define export symbols
#ifdef source
#define export  global
#else
#define export  extern
#endif
        endm

repeat  macro                           ; repeat until done
        goto    $ - 1
        endm

movff   macro   source, target          ; assign source to target file
        movfw   source                  ; W = source
        movwf   target                  ; target = W
        endm

movlf   macro   literal, file           ; assign literal to file
#if literal == 0
        clrf    file                    ; file = 0
#else
        movlw   literal                 ; w = literal
        movwf   file                    ; file = W
#endif
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
; Bank switching

    variable ACTIVE_BANK = -1 
    
mybank  macro   bank                    ; assume we are in this file bank
        variable ACTIVE_BANK = bank
        endm
        
forcsel macro   file                    ; switch file bank unconditionally
        banksel file
        mybank  file / BANK_SIZE
        endm
        
select  macro   file                    ; switch file bank only if needed
        local NEW_BANK = file / BANK_SIZE        
#if NEW_BANK != ACTIVE_BANK
        forcsel file
#endif
        endm
        
;===============================================================================
; FSR high bits switching
        
    variable ACTIVE_FSRH = -1 

myfsrh  macro   fsrh                    ; assume FSRH is set to this
        variable ACTIVE_FSRH = fsrh
        endm
        
forcisel macro file                     ; set FSRH unconditionally
        local   NEW_FSRH = file >> 8       
#ifdef HAVE_FSRH
        movlf   NEW_FSRH, FSRH
#else
        bankisel file
#endif
        myfsrh  NEW_FSRH
        endm
        
iselect macro   file                    ; set FSRH only if needed
        local NEW_FSRH = file >> 8        
#if NEW_FSRH != ACTIVE_FSRH
        forcisel file
#endif
        endm

;===============================================================================
; Inline and exceptions

inline  macro   function                ; call inline function
        function
        endm

try     macro   function                ; call function that can throw
        function
        endm

#define throw   goto                    ; throw exception

catch   macro   exception               ; catch exception
exception:
        endm

#endif
