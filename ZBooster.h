#ifndef zbooster_h
#define zbooster_h

#include "Energia.h"

#if defined(__MSP430G2553)
	#define ZBOOSTER_RGBLED_RED P2_1
	#define ZBOOSTER_RGBLED_GREEN P2_5
	#define ZBOOSTER_RGBLED_BLUE P2_6
	#define ZBOOSTER_BUTTON_SWITCH2 P1_3
#endif
#if defined(__MSP430FR5969)
	#define ZBOOSTER_RGBLED_RED P3_5
	#define ZBOOSTER_RGBLED_GREEN P1_5
	#define ZBOOSTER_RGBLED_BLUE P1_2
	#define ZBOOSTER_BUTTON_SWITCH2 P4_3
#endif
#if defined(__MSP430F5529)
	#define ZBOOSTER_RGBLED_RED P4_2
	#define ZBOOSTER_RGBLED_GREEN P2_6
	#define ZBOOSTER_RGBLED_BLUE P2_0
	#define ZBOOSTER_BUTTON_SWITCH2 P1_6
#endif
#if defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM____) 
	#define ZBOOSTER_RGBLED_RED PA_6
	#define ZBOOSTER_RGBLED_GREEN PA_4
	#define ZBOOSTER_RGBLED_BLUE PB_2
	#define ZBOOSTER_BUTTON_SWITCH2 PE_4
#endif
#if defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__) 
	#define ZBOOSTER_RGBLED_RED PB_2
	#define ZBOOSTER_RGBLED_GREEN PN_2
	#define ZBOOSTER_RGBLED_BLUE PM_3
	#define ZBOOSTER_BUTTON_SWITCH2 PE_2
#endif
#if defined(TARGET_IS_CC3101)
	#define ZBOOSTER_RGBLED_RED 9
	#define ZBOOSTER_RGBLED_GREEN 13
	#define ZBOOSTER_RGBLED_BLUE 19
	#define ZBOOSTER_BUTTON_SWITCH2 5
#endif
class ZBoosterClass
{
private:
public:
	ZBoosterClass();
	void begin();
};

extern ZBoosterClass ZBooster;

#endif