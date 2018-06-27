#include "pic.inc"

#ifdef __16F887
 __config _CONFIG1, _INTOSCIO & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOR_OFF & _IESO_OFF & _FCMEN_OFF & _LVP_OFF & _DEBUG_OFF
 __config _CONFIG2, _BOR40V & _WRT_OFF 
#endif
    
#ifdef __16F18313
 __config _CONFIG1, _FEXTOSC_OFF & _RSTOSC_HFINT32 & _CLKOUTEN_OFF & _CSWEN_ON & _FCMEN_OFF
 __config _CONFIG2, _MCLRE_OFF & _PWRTE_ON & _WDTE_OFF & _LPBOREN_OFF & _BOREN_OFF & _BORV_LOW & _PPS1WAY_OFF & _STVREN_ON & _DEBUG_OFF
 __config _CONFIG3, _WRT_OFF & _LVP_OFF
 __config _CONFIG4, _CP_OFF & _CPD_OFF
#endif

 end
