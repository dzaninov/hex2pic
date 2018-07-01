#ifndef UTIL_INC
#define UTIL_INC

#include "config.inc"

reboot  macro                           ; reboot pic
        inline  disable_int
        setpage 0
        goto    0
        endm

#endif
