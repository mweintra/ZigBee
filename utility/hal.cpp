
#include <stdint.h>
#include <stdbool.h>  //Required for driverlib compatibility
#include "Energia.h"
#include "HAL.h"
#include "../../SPI/SPI.h"
#include "hal_version.h"

halConfiguration hal;

/** Initializes Ports/Pins: sets direction, interrupts, pullup/pulldown resistors etc. */
void halInit()
{
	pinMode(hal.mrdyPin,OUTPUT);	//SS & MRDY
	pinMode(hal.srdyPin,INPUT);	// SRDY
	pinMode(hal.mrstPin,OUTPUT);	//MRST
    RADIO_OFF();
    SPI_SS_CLEAR();
}


/** Display information about this driver firmware */
void displayVersion()
{
    int i = 0;
    printf("\r\n");
    for (i=0; i<8; i++)
        printf("-");
    printf(" Module Interface and Examples %s ", MODULE_INTERFACE_STRING);
    for (i=0; i<8; i++)
            printf("-");
    printf("\r\n");
    printf("%s", MODULE_VERSION_STRING);
}

/* Setup for standard launchpads */
void halSpiInitModule()
{

#if defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM____) || defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__) 
	SPI.setModule(hal.spiModule);
#else
	SPI.begin();
#endif
	SPI.setDataMode(SPI_MODE0);
	
#if defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__)
	SPI.setClockDivider(SPI_CLOCK_DIV2); //1 MHz SPI Clock  
#elseif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM____)
	SPI.setClockDivider(SPI_CLOCK_DIV8); //1 MHz SPI Clock  
#else
	SPI.setClockDivider(SPI_CLOCK_DIV8); //1 MHz SPI Clock  
#endif
    // Don't select the module
    SPI_SS_CLEAR();
}

void halSpiReset(){
	SPI.end();
	halSpiInitModule();
}

/* Uses Energia SPI Functions */
void spiWrite(unsigned char *bytes, unsigned char numBytes)
{
	uint8_t ulReadData;
	while(numBytes--)    // Loop while there are more bytes left to be transferred.
    {
		ulReadData=SPI.transfer(*bytes);
        *bytes++ = (unsigned char)ulReadData;       // ...and then convert it to a char
    }
}


/* For compatibility */
void delayMs(uint16_t ms)
{
	delay(ms);
}
