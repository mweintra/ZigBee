/**
* @file zm_phy_spi.c
*
* @brief Physical Interface Layer to the Module using the Serial Peripheral Interface (SPI) port.
* This file acts as an interface between the module library methods and the hardware (hal file).
* 
* @note To output SPI transmit information, define ZM_PHY_SPI_VERBOSE.
* It is recommended to define this on a per-project basis. 
* In IAR, this can be done in Project Options : C/C++ compiler : Preprocessor
* In the defined symbols box, add:
* ZM_PHY_SPI_VERBOSE
*
* Normally the module is quite fast in responding to a SREQ and there is not sufficient time on a 
* slower processor (e.g. < 25MHz) to configure a timeout to detect a missing response. If using a 
* fast processor then define FAST_PROCESSOR. This does several things:
*   1. sendSreq() will timeout if a response was not received in time. 
*   2. the amount of time spent in each part of the SREQ process is available in variables 
*       timeFromChipSelectToSrdyLow and timeWaitingForSrsp.
*
* $Rev: 1796 $
* $Author: dsmith $
* $Date: 2013-04-22 03:00:33 -0700 (Mon, 22 Apr 2013) $
*
* @section support Support
* Please refer to the wiki at www.anaren.com/air-wiki-zigbee for more information. Additional support
* is available via email at the following addresses:
* - Questions on how to use the product: AIR@anaren.com
* - Feature requests, comments, and improvements:  featurerequests@teslacontrols.com
* - Consulting engagements: sales@teslacontrols.com
*
* @section license License
* Copyright (c) 2012 Tesla Controls. All rights reserved. This Software may only be used with an 
* Anaren A2530E24AZ1, A2530E24CZ1, A2530R24AZ1, or A2530R24CZ1 module. Redistribution and use in 
* source and binary forms, with or without modification, are subject to the Software License 
* Agreement in the file "anaren_eula.txt"
* 
* YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” 
* WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
* WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO 
* EVENT SHALL ANAREN MICROWAVE OR TESLA CONTROLS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, 
* STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR 
* INDIRECT DAMAGES OR EXPENSE INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, 
* PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE 
* GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY 
* DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/

#include "hal.h"
#include "zm_phy_spi.h"
#include "../../SPI/spi.h"
#include "module_errors.h"
#include <stdint.h>

//#define ZM_PHY_SPI_VERBOSE_ERRORS
/** This buffer will hold the transmitted messages and received SRSP Payload after sendMessage() was 
called. If fragmentation support is not required then this can be smaller, e.g. 100B.*/
uint8_t zmBuf[ZIGBEE_MODULE_BUFFER_SIZE];
#if defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM____) || defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__) || defined(TARGET_IS_CC3101)
#define FAST_PROCESSOR
#endif
#ifdef FAST_PROCESSOR           //used to report the amount of time it takes for the Module to respond over SPI
uint32_t timeFromChipSelectToSrdyLow = 0;
uint32_t timeWaitingForSrsp = 0;
//these will be used in sendSreq() for the timeouts:
#define CHIP_SELECT_TO_SRDY_LOW_TIMEOUT (TICKS_PER_MS * 125)    
#define WAIT_FOR_SRSP_TIMEOUT (TICKS_PER_MS * 125)              //typically takes less than 100mSec

#endif

/* Initializes the module PHY interface.
*/
void zm_phy_init()
{
  halSpiInitModule();
}

/** Whether the module has a message waiting to be retrieved.
 @return true (1) if there is a complete message ready for processing, or 0 otherwise.
*/
uint8_t moduleHasMessageWaiting()
{
  return (SRDY_IS_LOW());
}

/**
Sends a Module Synchronous Request (SREQ) message and retrieves the response. A SREQ is a message to 
the Module that is immediately followed by a Synchronous Response (SRSP) message from the Module. As 
opposed to an Asynchronous Request (AREQ) message, which does not have a SRSP. This is a private 
method that gets wrapped by sendMessage() and spiPoll().
@pre Module has been initialized
@pre zmBuf contains a properly formatted message. No validation is done.
@post received data is written to zmBuf
@return if FAST_PROCESSOR is defined then MODULE_SUCCESS, else an error code. If FAST_PROCESSOR is not defined, then MODULE_SUCCESS.
@note if running on a fast processor then you may need to modify the code to ensure that the module keeps up. See below
*/
moduleResult_t sendSreq()
{
#ifdef FAST_PROCESSOR                           //NOTE: only enable if using a processor with sufficient speed (25MHz+)
  uint32_t timeLeft1 = CHIP_SELECT_TO_SRDY_LOW_TIMEOUT;
  uint32_t timeLeft2 = WAIT_FOR_SRSP_TIMEOUT;
  
  SPI_SS_SET();                               // Assert SS
  while (SRDY_IS_HIGH() && (timeLeft1 != 0))  //wait until SRDY goes low
    timeLeft1--;
  if (timeLeft1 == 0)                         //SRDY did not go low in time, so return an error
    return ZM_PHY_CHIP_SELECT_TIMEOUT;
  timeFromChipSelectToSrdyLow = (CHIP_SELECT_TO_SRDY_LOW_TIMEOUT - timeLeft1);
  spiWrite(zmBuf, (*zmBuf + 3));              // *bytes (first byte) is length after the first 3 bytes, all frames have at least the first 3 bytes
  *zmBuf = 0; *(zmBuf+1) = 0; *(zmBuf+2) = 0; //poll message is 0,0,0
  //NOTE: MRDY must remain asserted here, but can de-assert SS if the two signals are separate
  
  /* Now: Data was sent, so we wait for Synchronous Response (SRSP) to be received.
  This will be indicated by SRDY transitioning to high */
  
  while (SRDY_IS_LOW() && (timeLeft2 != 0))    //wait for data
    timeLeft2--;
  if (timeLeft2 == 0)
    return ZM_PHY_SRSP_TIMEOUT;
  
  timeWaitingForSrsp = (WAIT_FOR_SRSP_TIMEOUT - timeLeft2);
  //NOTE: if SS & MRDY are separate signals then can re-assert SS here.
/*
    spiWrite(zmBuf, 1);
    if (*zmBuf > 0)                                 // *bytes (first byte) contains number of bytes to receive
        spiWrite(zmBuf+1, *zmBuf+2);                //write-to-read: read data into buffer                                  
    else
        spiWrite(zmBuf+1,2);
  */
  spiWrite(zmBuf, 3);
  //delay(5);
  /* NOTE: if on a fast processor, may need a little delay here */
  if (*zmBuf > 0)                             // *bytes (first byte) contains number of bytes to receive
    spiWrite(zmBuf+3, *zmBuf);              //write-to-read: read data into buffer
  SPI_SS_CLEAR();
  return 0;
#else                                       // In a slow processor there's not enough time to set up the timeout so there will be errors
  SPI_SS_SET();   
  while (SRDY_IS_HIGH()) ;   //wait until SRDY goes low

  spiWrite(zmBuf, (*zmBuf + 3));              // *bytes (first byte) is length after the first 3 bytes, all frames have at least the first 3 bytes
  *zmBuf = 0; *(zmBuf+1) = 0; *(zmBuf+2) = 0; //poll message is 0,0,0
  //NOTE: MRDY must remain asserted here, but can de-assert SS if the two signals are separate
  //Now: Data was sent, wait for Synchronous Response (SRSP)

  while (SRDY_IS_LOW()) ;                     //wait for data
  //NOTE: if SS & MRDY are separate signals then can re-assert SS here.

  spiWrite(zmBuf, 3);
  if (*zmBuf > 0)                             // *bytes (first byte) contains number of bytes to receive
    spiWrite(zmBuf+3, *zmBuf);              //write-to-read: read data into buffer    
  SPI_SS_CLEAR();                             // re-assert MRDY and SS
  return MODULE_SUCCESS;  
#endif
}

/**
Polls the Module for data. This is used to receive data from the Module, for example when a message 
has arrived. This will be initiated by detecting SRDY going low. 
@pre Module has been initialized.
@pre SRDY has gone low
@post received data is written to zmBuf
@note this method not required to be implemented when using UART interface.
*/
moduleResult_t getMessage()
{
  *zmBuf = 0; *(zmBuf+1) = 0; *(zmBuf+2) = 0;  //poll message is 0,0,0 
  return(sendSreq());
}

/** Public method to send messages to the Module. This will send one message and then receive the 
Synchronous Response (SRSP) message from the Module to indicate the command was received.
@pre zmBuf contains a properly formatted message
@pre Module has been initialized
@post buffer zmBuf contains the response (if any) from the Module. 
*/
moduleResult_t sendMessage()
{
#ifdef ZM_PHY_SPI_VERBOSE    
  printf("Tx: ");
  printHexBytes(zmBuf, zmBuf[0] + 3);
#endif    
  
  uint8_t expectedSrspCmdMsb = zmBuf[1] + SRSP_OFFSET;    //store these so we can compare with what is returned
  uint8_t expectedSrspCmdLsb = zmBuf[2];
  
  moduleResult_t result = sendSreq();                     //send message, buffer now holds received data
  
  if (result != MODULE_SUCCESS)                           //ERROR - sendSreq() timeout
  {
#ifdef ZM_PHY_SPI_VERBOSE_ERRORS    
    printf("ERROR - sreq() timeout %02X\r\n", result);
#endif 
	halSpiReset();
    return result;
  }
  
  /* The correct SRSP will always be 0x4000 + cmd, or simpler 0x4000 | cmd
  For example, if the SREQ is 0x2605 then the corresponding SRSP is 0x6605 */
  if ((zmBuf[SRSP_CMD_MSB_FIELD] == expectedSrspCmdMsb) && (zmBuf[SRSP_CMD_LSB_FIELD] == expectedSrspCmdLsb))    //verify the correct SRSP was received
  {
    return MODULE_SUCCESS;
  } else {
#ifdef ZM_PHY_SPI_VERBOSE_ERRORS    
    printf("ERROR - Wrong SRSP - received %02X-%02X, expected %02X-%02X\r\n", zmBuf[1], zmBuf[2],expectedSrspCmdMsb,expectedSrspCmdLsb);
#endif          
	halSpiReset();
    return ZM_PHY_INCORRECT_SRSP;   //Wrong SRSP received
  }
}

