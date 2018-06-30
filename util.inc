#ifndef UTIL_INC
#define UTIL_INC

#include "asm.inc"
#include "interrupt.inc"

reboot  macro                           ; reboot pic
        inline  disable_int
        setpage 0
        goto    0
        endm

#endif
