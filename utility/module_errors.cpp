/**
* @file module_errors.c
*
* @brief Module error handling. In one file here to make it easy to include wherever the module is used.
*
* $Rev: 1822 $
* $Author: dsmith $
* $Date: 2013-05-23 09:56:32 -0700 (Thu, 23 May 2013) $
*
* @section support Support
* Please refer to the wiki at www.anaren.com/air-wiki-zigbee for more information. Additional support
* is available via email at the following addresses:
* - Questions on how to use the product: AIR@anaren.com
* - Feature requests, comments, and improvements:  featurerequests@teslacontrols.com
* - Consulting engagements: sales@teslacontrols.com
*
* @section license License
* Copyright (c) 2013 Tesla Controls. All rights reserved. This Software may only be used with an 
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

#include "module_errors.h"
#include "hal.h"
#include <stdint.h>


moduleResult_t moduleResult = MODULE_SUCCESS;
//#define VERBOSE_ERROR_HANDLING

/** 
Converts error code to a string containing the name of the error.
@param err the error code
@return the name of the error, if known, or "Other Error" if unknown.
*/
char* getErrorName(moduleResult_t err)
{
    switch (err)
    {
    case INVALID_PARAMETER:
        return ("INVALID_PARAMETER");
    case NULL_PARAMETER:
        return ("NULL_PARAMETER");
    case TIMEOUT:
        return ("TIMEOUT");
    case INVALID_LENGTH:
        return ("INVALID_LENGTH");
    case INVALID_CLUSTER:
        return ("INVALID_CLUSTER");
    case ZM_PHY_CHIP_SELECT_TIMEOUT:
        return ("ZM_PHY_CHIP_SELECT_TIMEOUT");
    case ZM_PHY_SRSP_TIMEOUT:
        return ("ZM_PHY_SRSP_TIMEOUT");
    case ZM_PHY_INCORRECT_SRSP:
        return ("ZM_PHY_INCORRECT_SRSP");
    case ZM_INVALID_MODULE_CONFIGURATION:
        return ("ZM_INVALID_MODULE_CONFIGURATION");
    case ZM_PHY_OTHER_ERROR:
        return ("ZM_PHY_OTHER_ERROR");   
    default:
        return ("Other Error");
    }
}


/** 
The error handling method called if VERBOSE_ERROR_HANDLING is defined. You can customize this if desired.
@param methodId which method caused the error
@param errorCode the cause of the error
*/
void handleError(moduleResult_t errorCode, uint16_t methodId)
{
    printf("<Err 0x%02X (%s) in method 0x%04X>\r\n", errorCode, getErrorName(errorCode), methodId);
} // NOTE: to trap errors in the debugger then #define VERBOSE_ERROR_HANDLING and optionally set a hardware breakpoint right here

