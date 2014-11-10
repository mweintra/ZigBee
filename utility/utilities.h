/**
*  @file utilities.h
*
*  @brief  public methods for utilities.c
*
* $Rev: 1957 $
* $Author: dsmith $
* $Date: 2013-11-22 14:54:11 -0800 (Fri, 22 Nov 2013) $
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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include <stdbool.h>
#include "hal_version.h"

/** Convert two uint8_ts to an uint16_t, LSB first*/
#define CONVERT_TO_INT(lsb,msb) ((lsb) + 0x0100*(msb))   // ((lsb) + (((uint16_t) (msb)) << 8))

/** Get the Least Significant Byte (LSB) of an uint16_t*/
#define LSB(num) ((num) & 0xFF)

/** Get the Most Significant Byte (MSB) of an uint16_t*/
#define MSB(num) ((num) >> 8)

#define HINIBBLE(b) ((b)&0xF0) >> 4
#define LONIBBLE(b) ((b)&0x0F)

#define BYTES_TO_LONG(byteArray) (( ((unsigned long)byteArray[0] << 24) + ((unsigned long)byteArray[1] << 16) + ((unsigned long)byteArray[2] << 8) + ((unsigned long)byteArray[3] ) ) );



void initializeBuffer(uint8_t* buf, uint16_t len);
void printBinary(uint8_t n);
void printHexBytes(uint8_t* toPrint, uint16_t numBytes);
void displayHexBytes(const uint8_t* toPrint, const uint16_t numBytes, const char separator);
void displayReverseHexBytes(const uint8_t* toPrint, const uint16_t numBytes, const char separator);
char isAsciiPrintableCharacter(uint8_t c);
uint16_t getFirmwareVersion();

#define DISPLAY_HEX_BYTES_NO_SEPARATOR      0

//math functions
uint16_t getAverage(uint16_t* values, uint8_t numValues);
uint16_t roundInteger(uint16_t number, uint16_t place);
//uint16_t max(uint16_t values[], uint16_t numValues);
//uint16_t min(uint16_t values[], uint16_t numValues);


#define IS_NUMERIC_CHARACTER(c) ((c >= '0') && (c <= '9'))
#define IS_UPPER_CASE_HEXADECIMAL_CHARACTER(c) ((c >= 'A') && (c <= 'F'))
#define IS_LOWER_CASE_HEXADECIMAL_CHARACTER(c) ((c >= 'a') && (c <= 'f'))
#define IS_VALID_HEXADECIMAL_CHARACTER(c) ((IS_NUMERIC_CHARACTER(c)) || (IS_UPPER_CASE_HEXADECIMAL_CHARACTER(c)) || (IS_LOWER_CASE_HEXADECIMAL_CHARACTER(c)))
#define TO_UPPER_CASE(c) (((c >= 'a') && (c <= 'f')) ? (c -= 32) : (c))
#define CELSIUS_TO_FAHRENHEIT(c) (((9*c)/5) + 32)

#define DISPLAY_COMPILE_INFORMATION() (printf("File: %s, Date: %s, Time: %s\r\n", __FILE__, __DATE__, __TIME__))



#endif
