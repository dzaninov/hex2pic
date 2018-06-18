; shared
shared_data     udata_shr
#ifndef         HAVE_SHADOW
WREG_SHAD       res         1               ; context_save and restore
#endif

                udata
#ifndef         HAVE_SHADOW                 ; context_save and restore
STATUS_SHAD     res         1
PCLATH_SHAD     res         1
FSR_SHAD        res         1
#endif
