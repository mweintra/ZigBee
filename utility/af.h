/**
*  @file af.h
*
*  @brief  public methods for af.c
*
* $Rev: 1909 $
* $Author: dsmith $
* $Date: 2013-09-23 13:42:45 -0700 (Mon, 23 Sep 2013) $
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

#ifndef AF_H
#define AF_H

#include "application_configuration.h"
#include "module_errors.h"
#include <stdint.h>

uint8_t getTransactionSequenceNumber();
moduleResult_t afRegisterApplication(const struct applicationConfiguration* ac);
moduleResult_t afRegisterGenericApplication();
moduleResult_t afSendData(uint8_t destinationEndpoint, uint8_t sourceEndpoint,
                      uint16_t destinationShortAddress, uint16_t clusterId, 
                      uint8_t* data, uint8_t dataLength);
moduleResult_t afSendDataExtended(uint8_t destinationEndpoint, uint8_t sourceEndpoint,
                        uint8_t* destinationAddress, uint8_t destinationAddressMode,
                        uint16_t clusterId, uint8_t* data, uint16_t dataLength);
moduleResult_t afSendDataExtendedShort(uint8_t _destinationEndpoint, uint8_t _sourceEndpoint,
                                       uint16_t _destinationShortAddress, 
                                       uint16_t _clusterId, uint8_t* _data, uint16_t _dataLength);
moduleResult_t retrieveExtendedMessage(uint8_t* ts, uint16_t length, uint8_t* destinationPtr);

int16_t printAfIncomingMsgHeader(uint8_t* srsp);
void printAfIncomingMsgHeaderNames();

moduleResult_t afSetAckMode(uint8_t ackMode);
inline uint8_t getAckMode();

//For options field of afSendData()
#define AF_MAC_ACK                         0x00    //Require Acknowledgement from next device on route
#define AF_APS_ACK                      0x10    //Require Acknowledgement from final destination (if using AFZDO)

#define AF_INCOMING_MESSAGE_GROUP_LSB_FIELD             (SRSP_PAYLOAD_START)
#define AF_INCOMING_MESSAGE_GROUP_MSB_FIELD             (SRSP_PAYLOAD_START+1)
#define AF_INCOMING_MESSAGE_GROUP()                     (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_GROUP_LSB_FIELD], zmBuf[AF_INCOMING_MESSAGE_GROUP_MSB_FIELD]))
#define AF_INCOMING_MESSAGE_CLUSTER_LSB_FIELD           (SRSP_PAYLOAD_START+2)
#define AF_INCOMING_MESSAGE_CLUSTER_MSB_FIELD           (SRSP_PAYLOAD_START+3)
#define AF_INCOMING_MESSAGE_CLUSTER()                   (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_CLUSTER_LSB_FIELD], zmBuf[AF_INCOMING_MESSAGE_CLUSTER_MSB_FIELD]))
#define AF_INCOMING_MESSAGE_SHORT_ADDRESS_LSB_FIELD     (SRSP_PAYLOAD_START+4)
#define AF_INCOMING_MESSAGE_SHORT_ADDRESS_MSB_FIELD     (SRSP_PAYLOAD_START+5)
#define AF_INCOMING_MESSAGE_SHORT_ADDRESS()             (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_SHORT_ADDRESS_LSB_FIELD], zmBuf[AF_INCOMING_MESSAGE_SHORT_ADDRESS_MSB_FIELD]))
#define AF_INCOMING_MESSAGE_SOURCE_EP_FIELD     		(SRSP_PAYLOAD_START+6)
#define AF_INCOMING_MESSAGE_DESTINATION_EP_FIELD  		(SRSP_PAYLOAD_START+7)
#define AF_INCOMING_MESSAGE_WAS_BROADCAST_FIELD  		(SRSP_PAYLOAD_START+8)
#define AF_INCOMING_MESSAGE_LQI_FIELD                   (SRSP_PAYLOAD_START+9)
#define AF_INCOMING_MESSAGE_SECURITY_USE_FIELD          (SRSP_PAYLOAD_START+10)
#define AF_INCOMING_MESSAGE_TIMESTAMP_FIELD				(SRSP_PAYLOAD_START+11)
#define AF_INCOMING_MESSAGE_TRANSACTION_SEQUENCE_FIELD  (SRSP_PAYLOAD_START+15)
#define AF_INCOMING_MESSAGE_PAYLOAD_LEN_FIELD           (SRSP_PAYLOAD_START+16)
#define AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD         (SRSP_PAYLOAD_START+17)
#define IS_AF_INCOMING_MESSAGE()                        (CONVERT_TO_INT(zmBuf[SRSP_CMD_LSB_FIELD], zmBuf[SRSP_CMD_MSB_FIELD]) == AF_INCOMING_MSG)
#define AF_INCOMING_MESSAGE_PAYLOAD_LEN                 (zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_LEN_FIELD])

#define AF_INCOMING_MESSAGE_EXT_GROUP_LSB_FIELD         (SRSP_PAYLOAD_START)
#define AF_INCOMING_MESSAGE_EXT_GROUP_MSB_FIELD         (SRSP_PAYLOAD_START+1)
#define AF_INCOMING_MESSAGE_EXT_GROUP()                 (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_EXT_GROUP_LSB_FIELD], zmBuf[AF_INCOMING_MESSAGE_EXT_GROUP_MSB_FIELD]))
#define AF_INCOMING_MESSAGE_EXT_CLUSTER_LSB_FIELD       (SRSP_PAYLOAD_START+2)
#define AF_INCOMING_MESSAGE_EXT_CLUSTER_MSB_FIELD       (SRSP_PAYLOAD_START+3)
#define AF_INCOMING_MESSAGE_EXT_CLUSTER()               (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_EXT_CLUSTER_LSB_FIELD], zmBuf[AF_INCOMING_MESSAGE_EXT_CLUSTER_MSB_FIELD]))
#define AF_INCOMING_MESSAGE_EXT_ADDRESSING_MODE_FIELD   (SRSP_PAYLOAD_START+4)
#define AF_INCOMING_MESSAGE_EXT_SHORT_ADDRESS_LSB_FIELD (SRSP_PAYLOAD_START+5)  //IF USING SHORT ADDRESSING
#define AF_INCOMING_MESSAGE_EXT_SHORT_ADDRESS_MSB_FIELD (SRSP_PAYLOAD_START+6)
#define AF_INCOMING_MESSAGE_EXT_SHORT_ADDRESS()         (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_EXT_SHORT_ADDRESS_LSB_FIELD], zmBuf[AF_INCOMING_MESSAGE_EXT_SHORT_ADDRESS_MSB_FIELD]))
#define AF_INCOMING_MESSAGE_EXT_ADDRESS_START_FIELD     (SRSP_PAYLOAD_START+5)  //IF USING LONG ADDRESSING
#define AF_INCOMING_MESSAGE_EXT_SOURCE_EP_FIELD     	(SRSP_PAYLOAD_START+13)
#define AF_INCOMING_MESSAGE_EXT_DESTINATION_EP_FIELD  	(SRSP_PAYLOAD_START+16)
#define AF_INCOMING_MESSAGE_EXT_WAS_BROADCAST_FIELD  	(SRSP_PAYLOAD_START+17)
#define AF_INCOMING_MESSAGE_EXT_LQI_FIELD               (SRSP_PAYLOAD_START+18)
#define AF_INCOMING_MESSAGE_EXT_SECURITY_USE_FIELD      (SRSP_PAYLOAD_START+19)
#define AF_INCOMING_MESSAGE_EXT_TIMESTAMP_START_FIELD   (SRSP_PAYLOAD_START+20) 
#define AF_INCOMING_MESSAGE_EXT_PAYLOAD_LEN_LSB_FIELD   (SRSP_PAYLOAD_START+25)
#define AF_INCOMING_MESSAGE_EXT_PAYLOAD_LEN_MSB_FIELD   (SRSP_PAYLOAD_START+26)
#define AF_INCOMING_MESSAGE_EXT_PAYLOAD_START_FIELD     (SRSP_PAYLOAD_START+27)
#define AF_INCOMING_MESSAGE_EXT_LENGTH()                (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_EXT_PAYLOAD_LEN_LSB_FIELD], zmBuf[AF_INCOMING_MESSAGE_EXT_PAYLOAD_LEN_MSB_FIELD]))

#define IS_AF_INCOMING_MESSAGE_EXT()                    (CONVERT_TO_INT(zmBuf[SRSP_CMD_LSB_FIELD], zmBuf[SRSP_CMD_MSB_FIELD]) == AF_INCOMING_MSG_EXT)

#define AF_DATA_RETRIEVE_SRSP_STATUS_FIELD              (SRSP_PAYLOAD_START)
#define AF_DATA_RETRIEVE_SRSP_LENGTH_FIELD              (SRSP_PAYLOAD_START+1)
#define AF_DATA_RETRIEVE_SRSP_PAYLOAD_START_FIELD       (SRSP_PAYLOAD_START+2)

#define AF_DATA_CONFIRM_STATUS_FIELD                    (SRSP_PAYLOAD_START)
#define AF_DATA_CONFIRM_ENDPOINT_FIELD                  (SRSP_PAYLOAD_START+1)
#define AF_DATA_CONFIRM_TRANS_ID_FIELD                  (SRSP_PAYLOAD_START+2)
#define IS_AF_DATA_CONFIRM()                            (CONVERT_TO_INT(zmBuf[SRSP_CMD_LSB_FIELD], zmBuf[SRSP_CMD_MSB_FIELD]) == AF_DATA_CONFIRM)

#define AF_DATA_CONFIRM_STATUS                          (zmBuf[AF_DATA_CONFIRM_STATUS_FIELD])
#define AF_DATA_CONFIRM_TRANS_ID                        (zmBuf[AF_DATA_CONFIRM_TRANS_ID_FIELD])


//for extended messages - AF_DATA_REQUEST_EXT, etc.
#define DESTINATION_ADDRESS_MODE_NONE					0
#define DESTINATION_ADDRESS_MODE_GROUP					1
#define DESTINATION_ADDRESS_MODE_SHORT					2
#define DESTINATION_ADDRESS_MODE_LONG					3
#define DESTINATION_ADDRESS_MODE_BROADCAST				0xFF

#define INTRA_PAN 0  //same PAN
#define AF_DATA_REQUEST_MAX_PAYLOAD_LENGTH      MAXIMUM_PAYLOAD_LENGTH
#define AF_DATA_REQUEST_EXT_MAX_PAYLOAD_LENGTH 230
#define MAXIMUM_AF_DATA_RETRIEVE_PAYLOAD_LENGTH 248
#define AF_DATA_REQUEST_EXT_MAX_TOTAL_PAYLOAD_LENGTH 600

#endif
