#define INTERRUPT_ASM
#include "interrupt.inc"

#ifndef HAVE_SHADOW

 global WREG_SHAD
 global STATUS_SHAD
 global PCLATH_SHAD
 global FSR_SHAD

shared_data     udata_shr
WREG_SHAD       res         1

                udata
STATUS_SHAD     res         1
PCLATH_SHAD     res         1
FSR_SHAD        res         1

#endif

 end
