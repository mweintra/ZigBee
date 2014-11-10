/**
*  @file zm_phy_spi.h
*
*  @brief  public methods for zm_phy_spi.c
*
* $Rev: 1767 $
* $Author: dsmith $
* $Date: 2013-03-07 14:53:05 -0800 (Thu, 07 Mar 2013) $
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

#ifndef ZM_PHY_SPI_H
#define ZM_PHY_SPI_H




#include <stdint.h>
#include "module_errors.h"

moduleResult_t sendMessage();
moduleResult_t getMessage();
#define MODULE_HAS_MESSAGE_WAITING()  (SRDY_IS_LOW())
uint8_t moduleHasMessageWaiting();
void zm_phy_init();

#ifdef __MSP430G2553
#define ZIGBEE_MODULE_BUFFER_SIZE  128       // AF_INCOMING_MSG_EXT is largest: 30B for header + 130B for 
#else
#define ZIGBEE_MODULE_BUFFER_SIZE  0xFF                                       // Fragmentation Demo payload + 2B for UART framing bytes = 162B
#endif

extern uint8_t zmBuf[ZIGBEE_MODULE_BUFFER_SIZE];

#define SRSP_BUFFER_SIZE        20
#define SRSP_HEADER_SIZE        3

//SRSP MSB is 0x40 greater than SREQ MSB
#define SRSP_OFFSET             0x40
//Note: these may be different for a UART phy
#define SRSP_PAYLOAD_START      3
#define SRSP_LENGTH_FIELD       0  
#define SRSP_CMD_LSB_FIELD      2
#define SRSP_CMD_MSB_FIELD      1

#endif
