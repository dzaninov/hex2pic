#ifndef ASM_INC
#define ASM_INC

    radix dec
    list b=4,n=0,st=OFF                 ; b: tab spaces
                                        ; n: lines per page
                                        ; st: symbol table dump
#ifdef __GPUTILS_VERSION_MAJOR
    variable BANKING_ERROR = 1206       ; gpasm
#else
    variable BANKING_ERROR = 302        ; mpasm
#endif

#include "pic.inc"

reboot  macro                           ; reboot pic
        goto    0
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
    
unbank  macro                           ; forget current bank
        variable ACTIVE_BANK = -1
        errorlevel +BANKING_ERROR
        endm
        
mybank  macro   file                    ; assume we are in this file bank
        variable ACTIVE_BANK = file / BANK_SIZE
        errorlevel -BANKING_ERROR
        endm
        
forcsel macro   file                    ; switch file bank unconditionally
        banksel file
        mybank  file
        endm
        
select  macro   file                    ; switch file bank only if needed
        local NEW_BANK = file / BANK_SIZE        
#if NEW_BANK != ACTIVE_BANK
        forcsel file
#endif
        endm
        
relsel  macro   file                    ; switch bank for relocatable
        banksel file
        unbank
        endm
        
;===============================================================================
; FSR high bits switching
        
 variable ACTIVE_FSRH = -1 

unfsrh  macro                           ; forget current bank
        variable ACTIVE_FSRH = -1
        endm
        
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
; Calls and exceptions

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

routine macro   name                    ; define a global routine
name
        global  name
        unbank
        unfsrh
        endm
        
lclcall macro   routine                 ; call routine in the same module
        call    routine
        unbank
        endm
        
farcall macro   routine                 ; call routine in another module
#ifndef NO_PAGESEL
        pagesel routine
#endif
        call    routine
        unbank
#ifndef NO_PAGESEL
        local   local_label
        pagesel local_label
local_label:
#endif
        endm

#endif
