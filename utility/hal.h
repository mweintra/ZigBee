
#ifndef HAL_H
#define HAL_H


#include "Energia.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "printf.h"

void halInit();
void halSpiInitModule();
void halSpiReset();
void spiWrite(uint8_t *bytes, uint8_t numBytes);
void displayVersion();
void delayMs(uint16_t ms);

struct halConfiguration{
	uint8_t mrstPin;
	uint8_t mrdyPin;
	uint8_t srdyPin;
	uint8_t spiModule;
};

extern halConfiguration hal;

/*
extern uint8_t MRSTpin;
extern uint8_t MRDYpin;
extern uint8_t SRDYpin;
extern uint8_t SPImodule;
*/

#define ON_NETWORK_LED          2
#define NETWORK_FAILURE_LED     1
#define SEND_MESSAGE_LED        3

//
//  OTHER
//

/** Get the processor clock frequency */
#define GET_MCLK_FREQ() (SysCtlClockGet())
#define TICKS_PER_MS	25000			// used in FAST_PROCESSOR option of zm_phy
#define TICKS_PER_US (TICKS_PER_MS / 1000)
//#define WAIT_WHILE_SPI_BUSY()  while ((HWREG(SSI0_BASE + SSI_O_SR)) & SSI_SR_BSY)   //wait while busy

// SysTick
#define SYSTICK_INTERVAL_MS     8


// Buttons
#define ANY_BUTTON          0xFF
#define BUTTON_0            0
#define BUTTON_1            1
#define BUTTON_2            2

/* Peripherals:
LED0 - Red LED on BoosterPack, PB5
LED1 - Blue of RGB LED on BoosterPack, PB2
Can also use the Green LED on BoosterPack, but it's shared with MISO
Note: For pretty colors we're using the RGB LED on the Stellaris LaunchPad, not the one on the BoosterPack,
since the one on the LaunchPad is controlled by hardware PWM.
*/

//Module RST = PE0
//Module MRDY = CS = PA5
//Module SRDY = PA7
//PRECONDITIONS: GPIO PORTS HAVE BEEN ENABLED & PINS WERE CONFIGURED: RST, MRDY as outputs; SRDY as input
#define RADIO_ON()                  ( digitalWrite(hal.mrstPin,HIGH) )
#define RADIO_OFF()                 ( digitalWrite(hal.mrstPin,LOW) )
#define SPI_SS_SET()                ( digitalWrite(hal.mrdyPin,LOW) ) 	//MRDY tied to CS in hardware?
#define SPI_SS_CLEAR()              ( digitalWrite(hal.mrdyPin,HIGH) )
#define SRDY_IS_HIGH()              ( digitalRead(hal.srdyPin)==HIGH )
#define SRDY_IS_LOW()               ( digitalRead(hal.srdyPin)==LOW )


#endif
