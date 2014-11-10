/**
*  @file zdo.h
*
*  @brief  public methods for zdo.c
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

#ifndef ZDO_H
#define ZDO_H

#include "application_configuration.h"
#include "module_errors.h"

moduleResult_t zdoStartApplication();
moduleResult_t zdoRequestIeeeAddress(uint16_t shortAddress, uint8_t requestType, uint8_t startIndex);
moduleResult_t zdoNetworkAddressRequest(uint8_t* ieeeAddress, uint8_t requestType, uint8_t startIndex);
void displayZdoAddressResponse(uint8_t* rsp);
void displayZdoEndDeviceAnnounce(uint8_t* announce);
moduleResult_t zdoUserDescriptorRequest(uint16_t destinationAddress, uint16_t networkAddressOfInterest);
moduleResult_t zdoNodeDescriptorRequest(uint16_t destinationAddress, uint16_t networkAddressOfInterest);
moduleResult_t zdoUserDescriptorSet(uint16_t destinationAddress, uint16_t networkAddressOfInterest, 
                                    uint8_t* userDescriptor, uint8_t userDescriptorLength);
void displayZdoUserDescriptorResponse(uint8_t* rsp);
void displayZdoNodeDescriptorResponse(uint8_t* rsp);
moduleResult_t zdoManagementPermitJoinRequest(uint16_t destinationAddress, uint8_t duration, uint8_t tcSignificance);
moduleResult_t zdoNetworkDiscoveryRequest(uint32_t channelMask, uint8_t scanDuration);
moduleResult_t zdoManagementLeaveRequest(uint8_t* ieeeAddress, uint16_t destinationAddress);
moduleResult_t zdoRequestBind(uint16_t dstAddr, uint8_t* srcAddress, uint8_t srcEndpoint, uint16_t clusterId, uint8_t dstAddressMode, uint8_t* dstAddress, uint8_t dstEndpoint, uint8_t bind);



#define SINGLE_DEVICE_RESPONSE                          0
#define INCLUDE_ASSOCIATED_DEVICES                      1

#define BIND											0x00
#define UNBIND											0x01

#define DESTINATION_ADDRESS_MODE_NONE					0
#define DESTINATION_ADDRESS_MODE_GROUP					1
#define DESTINATION_ADDRESS_MODE_SHORT					2
#define DESTINATION_ADDRESS_MODE_LONG					3
#define DESTINATION_ADDRESS_MODE_BROADCAST				0xFF


//for ZDO_END_DEVICE_ANNCE_IND
#define FROM_ADDRESS_LSB                                (SRSP_PAYLOAD_START)
#define FROM_ADDRESS_MSB                                (SRSP_PAYLOAD_START+1)
#define SRC_ADDRESS_LSB                                 (SRSP_PAYLOAD_START+2)
#define SRC_ADDRESS_MSB                                 (SRSP_PAYLOAD_START+3)
#define IS_ZDO_END_DEVICE_ANNCE_IND()                   (CONVERT_TO_INT(zmBuf[SRSP_CMD_LSB_FIELD], zmBuf[SRSP_CMD_MSB_FIELD]) == ZDO_END_DEVICE_ANNCE_IND)
#define GET_ZDO_END_DEVICE_ANNCE_IND_FROM_ADDRESS()     (CONVERT_TO_INT(zmBuf[FROM_ADDRESS_LSB], zmBuf[FROM_ADDRESS_MSB]))
#define GET_ZDO_END_DEVICE_ANNCE_IND_SRC_ADDRESS()      (CONVERT_TO_INT(zmBuf[SRC_ADDRESS_LSB], zmBuf[SRC_ADDRESS_MSB]))
#define ZDO_END_DEVICE_ANNCE_IND_MAC_START_FIELD        (SRSP_PAYLOAD_START+4)
#define ZDO_END_DEVICE_ANNCE_IND_CAPABILITIES_FIELD                  (SRSP_PAYLOAD_START + 12)

#define ZDO_END_DEVICE_ANNCE_IND_CAPABILITIES_FLAG_DEVICETYPE_ROUTER     0x02
#define ZDO_END_DEVICE_ANNCE_IND_CAPABILITIES_FLAG_MAINS_POWERED         0x04
#define ZDO_END_DEVICE_ANNCE_IND_CAPABILITIES_FLAG_RX_ON_WHEN_IDLE       0x08
#define ZDO_END_DEVICE_ANNCE_IND_CAPABILITIES_FLAG_SECURITY_CAPABILITY   0x40


#define ZDO_USER_DESC_RSP_STATUS_FIELD                  (SRSP_PAYLOAD_START + 2)
#define ZDO_NODE_DESC_RSP_STATUS_FIELD                  (SRSP_PAYLOAD_START + 2)

#define ZDO_IEEE_ADDR_RSP_STATUS_FIELD                  (SRSP_PAYLOAD_START)
#define ZDO_NWK_ADDR_RSP_STATUS_FIELD                   (SRSP_PAYLOAD_START)

#define ZDO_MGMT_PERMIT_JOIN_RSP_STATUS_FIELD           (SRSP_PAYLOAD_START + 2)
#define ZDO_MGMT_LEAVE_RSP_STATUS_FIELD                 (SRSP_PAYLOAD_START + 2)

// For ZDO_IEEE_ADDR_RSP
#define ZDO_IEEE_ADDR_RSP_SHORT_ADDRESS_FIELD_START             9
#define ZDO_IEEE_ADDR_RSP_START_INDEX_FIELD                     11
#define ZDO_IEEE_ADDR_RSP_NUMBER_OF_ASSOCIATED_DEVICES_FIELD    12
#define ZDO_IEEE_ADDR_RSP_ASSOCIATED_DEVICE_FIELD_START         13

// For ZDO_BIND_RSP && ZDO_UNBIND_RSP
#define ZDO_BIND_SOURCE_ADDRESS_LSB                             (SRSP_PAYLOAD_START)
#define ZDO_BIND_SOURCE_ADDRESS_MSB                             (SRSP_PAYLOAD_START+1)
#define ZDO_BIND_RSP_STATUS_FIELD                   			(SRSP_PAYLOAD_START + 2)


// For MGMT_PERMIT_JOIN duration parameter
#define PERMIT_JOIN_OFF                 0x00
#define PERMIT_JOIN_ON_INDEFINITELY     0xFF


// For ZDO_NWK_DISCOVERY_REQ
#define IS_VALID_BEACON_ORDER(x)    (x<MAX_BEACON_ORDER)
#define MAX_BEACON_ORDER            (BEACON_ORDER_4_MINUTES)

#define BEACON_ORDER_NO_BEACONS     15
#define BEACON_ORDER_4_MINUTES      14  // 245760 milliseconds
#define BEACON_ORDER_2_MINUTES      13  // 122880 milliseconds
#define BEACON_ORDER_1_MINUTE       12  //  61440 milliseconds
#define BEACON_ORDER_31_SECONDS     11  //  30720 milliseconds
#define BEACON_ORDER_15_SECONDS     10  //  15360 MSecs
#define BEACON_ORDER_7_5_SECONDS     9  //   7680 MSecs
#define BEACON_ORDER_4_SECONDS       8  //   3840 MSecs
#define BEACON_ORDER_2_SECONDS       7  //   1920 MSecs
#define BEACON_ORDER_1_SECOND        6  //    960 MSecs
#define BEACON_ORDER_480_MSEC        5
#define BEACON_ORDER_240_MSEC        4
#define BEACON_ORDER_120_MSEC        3
#define BEACON_ORDER_60_MSEC         2
#define BEACON_ORDER_30_MSEC         1
#define BEACON_ORDER_15_MSEC                 0


#endif
