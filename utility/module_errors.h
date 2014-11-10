/**
* @file module_errors.h
*
* @brief Simple definitions of Module error codes returned in all methods that return moduleResult_t.
*
* @section about_the_error_handling_system About the Error Handling System
* The module library has a powerful and flexible error handling system. Each method in the library
* has a unique methodId:
 - module.c: 0x0100 .. 0x1F00
 - af.c: 0x2000 .. 0x2F00
 - zdo.c: 0x3000 .. 0x3F00 AND 0x7000 .. 0x7F00
 - simple_api.c: 0x4000 .. 0x4F00
 - Reserved 0x5000 .. 0x5F00
 - module_utilities.c 0x6000 .. 0x6F00

Also, there are different error codes depending on what caused the error. These are divided into
two types of errors:
 - Underlying Zigbee Stack errors
 - Module Interface errors
Both of these are listed in this file.
*
* @section how_to_use_error_handling How to use the Error Handling System
* If an error occurs, the method calls the HANDLE_ERROR(errorCode, methodId) macro. The behavior of 
* this macro depends on whether the preprocessor directive VERBOSE_ERROR_HANDLING is defined.
- if VERBOSE_ERROR_HANDLING is defined: the macro will call the function handleError(errorCode, methodId) in module_errors.c. This method can be customized for your purposes.
- if VERBOSE_ERROR_HANDLING is not defined: nothing will happen. 
*
* @section zstack_errors Errors from low level Zigbee Stack
<pre>
//Note: from ZComDef.h:

// Redefined Generic Status Return Values for code backwards compatibility
#define ZSuccess                    SUCCESS
#define ZFailure                    FAILURE
#define ZInvalidParameter           INVALIDPARAMETER

//From comdef.h - Generic Status Return Values
#define SUCCESS                   0x00
#define FAILURE                   0x01
#define INVALIDPARAMETER          0x02
#define INVALID_TASK              0x03
#define MSG_BUFFER_NOT_AVAIL      0x04
#define INVALID_MSG_POINTER       0x05
#define INVALID_EVENT_ID          0x06
#define INVALID_INTERRUPT_ID      0x07
#define NO_TIMER_AVAIL            0x08
#define NV_ITEM_UNINIT            0x09
#define NV_OPER_FAILED            0x0A
#define INVALID_MEM_SIZE          0x0B
#define NV_BAD_ITEM_LEN           0x0C

// ZStack status values must start at 0x10, after the generic status values (defined in comdef.h)
#define ZMemError                   0x10
#define ZBufferFull                 0x11
#define ZUnsupportedMode            0x12
#define ZMacMemError                0x13

#define ZSapiInProgress             0x20
#define ZSapiTimeout                0x21
#define ZSapiInit                   0x22

//0x30-0x6F - our module errors

#define ZNotAuthorized              0x7E

#define ZMalformedCmd               0x80
#define ZUnsupClusterCmd            0x81

#define ZStack_not_supported		0x84

// OTA Status values
#define ZOtaAbort                   0x95
#define ZOtaImageInvalid            0x96
#define ZOtaWaitForData             0x97
#define ZOtaNoImageAvailable        0x98
#define ZOtaRequireMoreImage        0x99

// Security status values
#define ZSecNoKey                   0xa1
#define ZSecOldFrmCount             0xa2
#define ZSecMaxFrmCount             0xa3
#define ZSecCcmFail                 0xa4

// APS status values
#define ZApsFail                    0xb1
#define ZApsTableFull               0xb2
#define ZApsIllegalRequest          0xb3
#define ZApsInvalidBinding          0xb4
#define ZApsUnsupportedAttrib       0xb5
#define ZApsNotSupported            0xb6
#define ZApsNoAck                   0xb7
#define ZApsDuplicateEntry          0xb8
#define ZApsNoBoundDevice           0xb9
#define ZApsNotAllowed              0xba
#define ZApsNotAuthenticated        0xbb
	
// NWK status values
#define ZNwkInvalidParam            0xc1
#define ZNwkInvalidRequest          0xc2
#define ZNwkNotPermitted            0xc3
#define ZNwkStartupFailure          0xc4
#define ZNwkAlreadyPresent          0xc5
#define ZNwkSyncFailure             0xc6
#define ZNwkTableFull               0xc7
#define ZNwkUnknownDevice           0xc8
#define ZNwkUnsupportedAttribute    0xc9
#define ZNwkNoNetworks              0xca
#define ZNwkLeaveUnconfirmed        0xcb
#define ZNwkNoAck                   0xcc  // not in spec
#define ZNwkNoRoute                 0xcd

//NOTE: 0xd0 .. 0xdf reserved for our module errors

// MAC status values
#define ZMacSuccess                 0x00
#define ZMacBeaconLoss              0xe0
#define ZMacChannelAccessFailure    0xe1
#define ZMacDenied                  0xe2
#define ZMacDisableTrxFailure       0xe3
#define ZMacFailedSecurityCheck     0xe4
#define ZMacFrameTooLong            0xe5
#define ZMacInvalidGTS              0xe6
#define ZMacInvalidHandle           0xe7
#define ZMacInvalidParameter        0xe8
#define ZMacNoACK                   0xe9
#define ZMacNoBeacon                0xea
#define ZMacNoData                  0xeb
#define ZMacNoShortAddr             0xec
#define ZMacOutOfCap                0xed
#define ZMacPANIDConflict           0xee
#define ZMacRealignment             0xef
#define ZMacTransactionExpired      0xf0
#define ZMacTransactionOverFlow     0xf1
#define ZMacTxActive                0xf2
#define ZMacUnAvailableKey          0xf3
#define ZMacUnsupportedAttribute    0xf4
#define ZMacUnsupported             0xf5
#define ZMacSrcMatchInvalidIndex    0xff
</pre>
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

#ifndef MODULE_ERRORS_H
#define MODULE_ERRORS_H

#include <stdint.h>

#define VERBOSE_ERROR_HANDLING

/** 
This is the return type from module methods. 
Standardized as one type here to make modules which return this easier to understand.
*/
typedef uint8_t moduleResult_t;

/* Temporary variable used to hold the result of error checking macros. This is used so that we can
perform the operation once, and then based on the returned value of said operation decide what to
do next. Only error macros use this variable; normal methods don't write it.*/
extern moduleResult_t moduleResult;

void handleError(moduleResult_t errorCode, uint16_t methodId);

/** This is the generic error handling method. Whether it calls handleError() is determined by whether VERBOSE_ERROR_HANDLING is defined. */
#ifdef VERBOSE_ERROR_HANDLING
  #define HANDLE_ERROR(errorCode, methodId) \
    handleError(errorCode, methodId);  //call error handling method
#else
  #define HANDLE_ERROR(errorCode, methodId) //be silent
#endif

/** 
Executes the code in operation once and returns the result of said operation. If result was an
error then calls the HANDLE_ERROR macro. 
@param operation may be a moduleResult_t type variable, or an 
operation that returns a moduleResult_t, for example sendMessage(). The operation is only executed once.
@param methodId which method caused the error
*/
#define RETURN_RESULT(operation, methodId) \
    moduleResult = operation; \
        if (moduleResult != MODULE_SUCCESS) { \
            HANDLE_ERROR(moduleResult, methodId); } \
                return moduleResult;

/** 
Tests an expression and if the expression evaluates to true then calls the HANDLE_ERROR macro and
causes the enclosed method to return the specified errorCode. If expression was false then doesn't do anything.
@note Since this does NOT copy the result of the expression, use only on logical expression, not function calls!
@param expression the expression to test. If true then error.
@param methodId which method caused the error
@param errorCode the cause of the error
*/
#define RETURN_RESULT_IF_EXPRESSION_TRUE(expression, methodId, errorCode) \
    if (expression) { \
        HANDLE_ERROR(errorCode, methodId); \
            return errorCode;        }

/** Utility macro to return a INVALID_PARAMETER error if the expression evaluates to true */
#define RETURN_INVALID_PARAMETER_IF_TRUE(_expression, _methodId)   RETURN_RESULT_IF_EXPRESSION_TRUE((_expression), (_methodId), INVALID_PARAMETER)

/** Utility macro to return a NULL_PARAMETER error if the expression evaluates to true */
#define RETURN_NULL_PARAMETER_IF_TRUE(_expression, _methodId)   RETURN_RESULT_IF_EXPRESSION_TRUE((_expression), (_methodId), NULL_PARAMETER)
    
/** Utility macro to return a INVALID_LENGTH error if the expression evaluates to true */
#define RETURN_INVALID_LENGTH_IF_TRUE(_expression, _methodId)   RETURN_RESULT_IF_EXPRESSION_TRUE((_expression), (_methodId), INVALID_LENGTH)

/** Utility macro to return a INVALID_CLUSTER error if the expression evaluates to true */
#define RETURN_INVALID_CLUSTER_IF_TRUE(_expression, _methodId)   RETURN_RESULT_IF_EXPRESSION_TRUE((_expression), (_methodId), INVALID_CLUSTER)

/** Executes the code in operation once, and if result is not success then calls the HANDLE_ERROR macro and
causes the enclosed method to return the specified errorCode. If operation was successful then 
doesn't do anything.  
@param operation the operation to execute. If fails then error.
@param methodId which method caused the error
*/
#define RETURN_RESULT_IF_FAIL(operation, methodId) \
    moduleResult = operation; \
        if (moduleResult != MODULE_SUCCESS) { \
            HANDLE_ERROR(moduleResult, methodId); \
                return moduleResult; }

//
//Error codes: These should not overlap Z-Stack error codes
//

/** The module operation completed successfully */
#define MODULE_SUCCESS                  (0x00)
                
/** A parameter was incorrect */
#define INVALID_PARAMETER	            (0x31)
                
/** The module driver timed out and could not process the request */
#define TIMEOUT                         (0x32)
                
/** The length of a parameter was incorrect */
#define INVALID_LENGTH                  (0x33)
                
/** The Zigbee Cluster parameter was incorrect */
#define INVALID_CLUSTER                 (0x34)

/** The parameter was NULL */
#define NULL_PARAMETER                  (0x35)
                
/** The Module did not respond to chip select being asserted.
@see Module physical interface files (e.g. zm_phy_spi.c) for more information*/
#define ZM_PHY_CHIP_SELECT_TIMEOUT      (0x37)
/** The Module did not respond with a Synchronous Response in time
@see Module physical interface files (e.g. zm_phy_spi.c) for more information*/
#define ZM_PHY_SRSP_TIMEOUT             (0x38)
/** The Synchronous Response returned did not match the Synchronous Request
@see Module physical interface files (e.g. zm_phy_spi.c) for more information*/
#define ZM_PHY_INCORRECT_SRSP           (0x39)
/** The module is not configured correctly. See startModule() in module_utilities.c */
#define ZM_INVALID_MODULE_CONFIGURATION (0x3A)
/** An error occured that doesn't fit into one of the other categories
@see Module physical interface files (e.g. zm_phy_spi.c) for more information*/
#define ZM_PHY_OTHER_ERROR              (0x3B)



#endif
