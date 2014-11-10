/**
* @file module_commands.h
*
* @brief Simple definitions of Module commands
*
* These are listed MSB first, as the bytes come "down the wire" instead of LSB first like everything else.
* @note you may want to print this out and keep it handy, as it is very useful when debugging.
* @note This is not a comprehensive list of all Module commands, only what is currently used by the
* library or seen when running the examples.
*
* @see <a href="http://teslacontrols.com/mw/index.php?title=Module_Interface_Specification">Module Interface Specification</a> 
* on the Wiki for more information on each command.
*
* $Rev: 1894 $
* $Author: dsmith $
* $Date: 2013-09-04 14:55:23 -0700 (Wed, 04 Sep 2013) $
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

#ifndef MODULE_COMMANDS_H
#define MODULE_COMMANDS_H

// SYS Interface
#define ZB_WRITE_CONFIGURATION          0x2605
#define ZB_READ_CONFIGURATION           0x2604
#define SYS_GPIO                        0x210E
#define SYS_VERSION                     0x2102
#define SYS_RANDOM                      0x210C
#define SYS_ADC_READ					0x210D

#define SYS_SET_TIME					0x2110
#define SYS_GET_TIME					0x2111

#define SYS_NV_READ                     0x2108
#define SYS_NV_WRITE                    0x2109
#define SYS_RESET_IND                   0x4180
#define SYS_STACK_TUNE                  0x210F
#define SYS_SET_TX_POWER                0x2114

// Simple API commands
#define ZB_APP_REGISTER_REQUEST         0x260A
#define ZB_APP_START_REQUEST            0x2600
#define ZB_SEND_DATA_REQUEST            0x2603
#define ZB_SEND_DATA_CONFIRM            0x4683 //will receive this asynchronously
#define ZB_GET_DEVICE_INFO              0x2606
#define ZB_FIND_DEVICE_REQUEST          0x2607
#define ZB_FIND_DEVICE_CONFIRM          0x4685
#define ZB_PERMIT_JOINING_REQUEST       0x2608
#define ZB_START_CONFIRM                0x4680 //will receive this asynchronously
#define ZB_RECEIVE_DATA_INDICATION      0x4687 //will receive this asynchronously

// AF commands:
#define AF_REGISTER                     0x2400
#define AF_DATA_REQUEST                 0x2401
#define AF_DATA_REQUEST_EXT             0x2402
#define AF_DATA_STORE                   0x2411
#define AF_DATA_RETRIEVE                0x2412
#define AF_DATA_CONFIRM                 0x4480 //will receive this asynchronously
#define AF_INCOMING_MSG                 0x4481 //will receive this asynchronously
#define AF_INCOMING_MSG_EXT             0x4482 //will receive this asynchronously

// ZDO commands:
#define ZDO_STARTUP_FROM_APP            0x2540
#define ZDO_IEEE_ADDR_REQ               0x2501
#define ZDO_IEEE_ADDR_RSP               0x4581
#define ZDO_NWK_ADDR_REQ                0x2500
#define ZDO_NWK_ADDR_RSP                0x4580
#define ZDO_USER_DESC_REQ               0x2508
#define ZDO_USER_DESC_RSP               0x4588
#define ZDO_USER_DESC_SET               0x250B
#define ZDO_USER_DESC_CONF              0x4589 //will receive this asynchronously
#define ZDO_NODE_DESC_REQ               0x2502
#define ZDO_NODE_DESC_RSP               0x4582
#define ZDO_MGMT_PERMIT_JOIN_REQ        0x2536
#define ZDO_MGMT_PERMIT_JOIN_RSP        0x45B6
#define ZDO_NWK_DISCOVERY_REQ           0x2526
#define ZDO_NWK_DISCOVERY_CONF          0x45C7
#define ZDO_BEACON_NOTIFY_IND           0x45C5
#define ZDO_MGMT_LEAVE_REQ              0x2534
#define ZDO_MGMT_LEAVE_RSP              0x45B4
#define ZDO_BIND_REQ					0x2521
#define ZDO_BIND_RSP					0x45A1
#define ZDO_UNBIND_REQ					0x2522
#define ZDO_UNBIND_RSP					0x45A2

#define ZDO_STATE_CHANGE_IND            0x45C0 //will receive this asynchronously
#define ZDO_END_DEVICE_ANNCE_IND        0x45C1 //will receive this asynchronously
#define ZDO_LEAVE_IND        		0x45C9 //will receive this asynchronously

// UTIL commands:
#define UTIL_ADDRMGR_NWK_ADDR_LOOKUP    0x2741

// Other commands:
#define ERROR_SRSP                      0x6000 // returned if SREQ is unknown

// Received in SRSP message
#define SRSP_STATUS_SUCCESS             0x00

// expected Response message types
#define NO_RESPONSE_EXPECTED            0x00

#endif
