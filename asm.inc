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

;===============================================================================
; New instructions
    
repeat  macro                           ; repeat until done
        goto    $ - 1
        endm

movff   macro   source, target          ; assign source to target file
        movfw   source                  ; W = source
        movwf   target                  ; target = W
        endm

rmovlf  macro   literal, file           ; assign relocatable literal to file
        movlw   literal                 ; w = literal
        movwf   file                    ; file = W
        endm

movlf   macro   literal, file           ; assign literal to file
#if literal == 0
        clrf    file                    ; file = 0
#else
        rmovlf  literal, file
#endif
        endm

shr     macro   file, destination       ; shift right
        bcf     STATUS, C               ; clear carry
        rrf     file, destination       ; rotate right
        endm

shl     macro   file, destination       ; shift left
        bcf     STATUS, C               ; clear carry
        rlf     file, destination       ; rotate left
        endm

letz    macro   state                   ; set Z to state
#if state == 0
        clrz
#else
        setz
#endif
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
        
fselect macro   file                    ; switch file bank unconditionally
        banksel file
        mybank  file
        endm
        
select  macro   file                    ; switch file bank only if needed
        local NEW_BANK = file / BANK_SIZE        
#if NEW_BANK != ACTIVE_BANK
        fselect file
#endif
        endm
        
rselect macro   file                    ; switch bank for relocatable
        banksel file
        unbank
        endm
        
;===============================================================================
; FSR high bits switching
        
 variable ACTIVE_BANKI = -1 

unbanki macro                           ; forget current index bank
        variable ACTIVE_BANKI = -1
        endm
        
mybanki macro   banki                   ; assume index bank is set to this
        variable ACTIVE_BANKI = banki
        endm
        
fselecti macro  file                    ; set index bank unconditionally
        local   NEW_BANKI = file >> 8       
#ifdef HAVE_FSRH
        movlf   NEW_BANKI, FSRH
#else
        bankisel file
#endif
        mybanki NEW_BANKI
        endm
        
selecti macro   file                    ; set index bank only if needed
        local   NEW_BANKI = file >> 8        
#if NEW_BANKI != ACTIVE_BANKI
        fselecti file
#endif
        endm

;===============================================================================
; Page switching

setpage macro   label                   ; set code page to access label
#ifndef ONE_PAGE
#if label == 0
        clrf    PCLATH
#else
        pagesel label
#endif
#endif
        endm

setrpage macro  label                   ; set code page for relocatable label
#ifndef ONE_PAGE
        pagesel label
#endif
        endm
    
unpage  macro                           ; set code page to local
        local   local_label
        setrpage local_label
local_label:
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
        unbanki
        endm
        
lclcall macro   routine                 ; call routine in the same module
        call    routine
        unbank
        endm
        
farcall macro   routine                 ; call routine in another module
        setrpage routine
        call    routine
        unbank
        unpage
        endm

retz    macro   state                   ; set Z to state and return
        letz    state
        return
        endm

reboot  macro                           ; reboot pic
        setpage 0
        goto    0
        endm

#endif
