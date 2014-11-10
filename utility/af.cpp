/**
* @file af.c
*
* @brief Methods that implement the Application Framework (AF) interface.
* 
* The AF/ZDO interface is a more powerful version of the Simple API and allows you to configure, 
* send, and receive Zigbee data. This file acts as an interface between the user's application and 
the Module physical interface. Module interface could be either SPI or UART.
* Refer to Interface Specification for more information.
*
* @note For more information, define AF_VERBOSE. It is recommended to define this on a per-project basis. 
* In IAR, this can be done in Project Options : C/C++ compiler : Preprocessor
* In the defined symbols box, add:
* AF_VERBOSE
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

#include "af.h"
#include "module.h"
#include "module_errors.h"
#include "hal.h"
#include "utilities.h"
#include "application_configuration.h"
#include "zm_phy_spi.h"
#include <string.h>                 //for memcpy()
#include <stdint.h>

extern uint8_t zmBuf[ZIGBEE_MODULE_BUFFER_SIZE];

/** Incremented for each AF_DATA_REQUEST, wraps around to 0. */
static uint8_t transactionSequenceNumber = 0;

/** This configures whether we want to receive an ACK from the next node or from the final
destination. Using APS ACK results in more traffic on the network. Most of the time MAC ACK is ok.
*/
static uint8_t acknowledgmentMode = AF_MAC_ACK;
//#define AF_VERBOSE
#define METHOD_AF_REGISTER_APPLICATION                    0x2100
/** 
 Configures the Module for our application.
Sets which profileId, etc. we're using as well as binding information FOR EACH ENDPOINT
Using the AF/ZDO interface, you can have more than one endpoint. Recommended not to exceed 10 endpoints.
@param ac the applicationConfiguration to register. Endpoint must NOT be zero.
@pre Module was initialized and ZCD_NV_LOGICAL_TYPE has been set (COORDINATOR/ROUTER/END_DEVICE).
@return MODULE_SUCCESS if success, else an error code
@post can now use afStartApplication()
@note this method does not check whether the endpoint being registered has already been registered - it is up to the application to manage that.
@see applicationConfiguration
*/
moduleResult_t afRegisterApplication(const struct applicationConfiguration* ac)
{
#ifdef AF_VERBOSE
    printf("Register Application Configuration with AF/ZDO\r\n");
#endif
    RETURN_INVALID_PARAMETER_IF_TRUE( (ac->endPoint == 0), METHOD_AF_REGISTER_APPLICATION);
    RETURN_NULL_PARAMETER_IF_TRUE( (ac == 0), METHOD_AF_REGISTER_APPLICATION);    
    RETURN_INVALID_CLUSTER_IF_TRUE( ((ac->numberOfBindingInputClusters > MAX_BINDING_CLUSTERS) || (ac->numberOfBindingOutputClusters > MAX_BINDING_CLUSTERS)), METHOD_AF_REGISTER_APPLICATION);
    
    //zmBuf[0] (length) will be set later
    zmBuf[1] = MSB(AF_REGISTER);
    zmBuf[2] = LSB(AF_REGISTER);  
    
    uint8_t bufferIndex;
    zmBuf[3] = ac->endPoint;
    zmBuf[4] = ac->profileId & 0xFF;      //LSB
    zmBuf[5] = ac->profileId >> 8;        //MSB
    zmBuf[6] = ac->deviceId & 0xFF;       //LSB
    zmBuf[7] = ac->deviceId >> 8;         //MSB
    zmBuf[8] = ac->deviceVersion;
    zmBuf[9] = ac->latencyRequested;
    zmBuf[10] = ac->numberOfBindingInputClusters;
    
    bufferIndex = 11;
    int cluster;
    for (cluster = 0; cluster < ac->numberOfBindingInputClusters; cluster++)
    {
        zmBuf[bufferIndex++] = ac->bindingInputClusters[cluster] & 0xFF;
        zmBuf[bufferIndex++] = ac->bindingInputClusters[cluster] >> 8;
    }
    zmBuf[bufferIndex++] = ac->numberOfBindingOutputClusters;
    
    for (cluster = 0; cluster < ac->numberOfBindingOutputClusters; cluster++)
    {
        zmBuf[bufferIndex++] = ac->bindingOutputClusters[cluster] & 0xFF;
        zmBuf[bufferIndex++] = ac->bindingOutputClusters[cluster] >> 8;
    }
    zmBuf[0] = bufferIndex;
    RETURN_RESULT(sendMessage(), METHOD_AF_REGISTER_APPLICATION); 
}


#define METHOD_AF_REGISTER_GENERIC_APPLICATION                    0x2200
/** Configures the Module for a "generic" application: one endpoint, no binding or fancy stuff. 
Sets which endpoint, profileId, etc. we're using as well as binding information
@return MODULE_SUCCESS if success, else an error code
@see afRegisterApplication()
*/
moduleResult_t afRegisterGenericApplication()
{
#define AF_REGISTER_PAYLOAD_LEN 13
    zmBuf[0] = AF_REGISTER_PAYLOAD_LEN;
    zmBuf[1] = MSB(AF_REGISTER);
    zmBuf[2] = LSB(AF_REGISTER);      
    
    zmBuf[3] = DEFAULT_ENDPOINT;
    zmBuf[4] = LSB(DEFAULT_PROFILE_ID);
    zmBuf[5] = MSB(DEFAULT_PROFILE_ID);
    zmBuf[6] = LSB(DEVICE_ID);
    zmBuf[7] = MSB(DEVICE_ID);
    zmBuf[8] = DEVICE_VERSION;
    zmBuf[9] = LATENCY_NORMAL;
    zmBuf[10] = 1; //number of binding input clusters
	zmBuf[11]=LSB(INFO_MESSAGE_CLUSTER);
	zmBuf[12]=MSB(INFO_MESSAGE_CLUSTER);	
    zmBuf[13] = 1; //number of binding output clusters
	zmBuf[14]=LSB(INFO_MESSAGE_CLUSTER);
	zmBuf[15]=MSB(INFO_MESSAGE_CLUSTER);	
    RETURN_RESULT(sendMessage(), METHOD_AF_REGISTER_GENERIC_APPLICATION);
}

/** Returns the current transactionSequenceNumber. This is included in afSendData, and reported in
AF_DATA_CONFIRM. Use this to match the AF_DATA_CONFIRM with calls to afSendData().
@return the current transactionSequenceNumber - this was sent with the last call to afSendData or afSendDataExt.
@note this is implemented as a method for modularity; to keep other code from changing this variable.
*/
uint8_t getTransactionSequenceNumber()
{
    return transactionSequenceNumber;
}

#define METHOD_AF_SET_ACK_MODE                    0x2900
/** Configures the module interface for either MAC level acking or application level acking.
This setting is used in afSendData() and afSendDataExt().
*/
moduleResult_t afSetAckMode(uint8_t ackMode)
{
    RETURN_INVALID_PARAMETER_IF_TRUE(((ackMode != AF_MAC_ACK) && (ackMode != AF_APS_ACK)), METHOD_AF_SET_ACK_MODE);
    acknowledgmentMode = ackMode;
    return MODULE_SUCCESS;
}

/** Retrieves the Acknowledgement mode.
 * @see afSetAckMode()
 * @return the Acknowledgement mode - either AF_MAC_ACK or AF_APS_ACK
 */
uint8_t getAckMode()
{
	return acknowledgmentMode;
}

#define METHOD_AF_SEND_DATA                    0x2300
/** Sends a message to another device over the Zigbee network using the AF command AF_DATA_REQUEST.
@param  destinationEndpoint which endpoint to send this to.
@param  sourceEndpoint which endpoint this message originated from on our device
@param  destinationShortAddress the short address of the destination, or ALL_DEVICES or 
ALL_ROUTERS_AND_COORDINATORS to broadcast the message.
@param  clusterId which cluster this message is for. User definable. Zigbee supports up to 2^16 clusters. 
A cluster is typically a particular command, e.g. "turn on lights" or "get temperature". If using a 
predefined Zigbee Alliance Application Profile then this cluster will follow the Zigbee Cluster Library.
@param  data is the data to send.
@param  dataLength is how many bytes of data to send. Must be nonzero and less than MAXIMUM_PAYLOAD_LENGTH.
@note On a coordinator in a trivial test setup, it takes approximately 10mSec from sending 
AF_DATA_REQUEST to when we receive AF_DATA_CONFIRM.
@note   When sending data, three things happen:
1. AF_DATA_REQUEST sent to module.
2. Module sends back AF_DATA_REQUEST SRSP to indicate the message was received.
3. Once message was delivered, Module sends back AF_DATA_CONFIRM.
For simplicity, this library function will use a blocking wait to wait for the AF_DATA_CONFIRM.
 Define AF_DATA_CONFIRM_HANDLED_BY_APPLICATION if checking for AF_DATA_CONFIRM messages in
the application instead of this method. Doing it in the application is faster, but requires more code.
@note   <code>transactionSequenceNumber</code> is an optional user-definable reference number to match AF_DATA_REQUEST messages with AF_DATA_CONFIRM messages. 
@note   The Module will automatically require an ACK from the next device on the route when sending data. 
To require an ACK from the final destination, use afSetAckMode to set acknowledgement mode to 
AF_APS_ACK at the expense of increased network traffic.
@note   The <code>radius</code> is the maximum number of hops that this packet can travel through 
before it will be dropped and should be set to the maximum number of hops expected in the network.
@note   adjust AF_DATA_CONFIRM_TIMEOUT  based on network size, number of hops, etc.
@pre    the application was started successfully
@pre    there is another device on the network with short address of <code>destinationShortAddress</code> 
and that device has successfully started its application.
@post   We will receive a AF_DATA_REQUEST_SRSP regardless of whether the message was successfully sent or not.
@post   we will receive a AF_DATA_CONFIRM if the message was successfully sent. 
*/
moduleResult_t afSendData(uint8_t destinationEndpoint, uint8_t sourceEndpoint, 
                          uint16_t destinationShortAddress, uint16_t clusterId, 
                          uint8_t* data, uint8_t dataLength)
{
    RETURN_INVALID_LENGTH_IF_TRUE( ((dataLength > MAXIMUM_PAYLOAD_LENGTH) || (dataLength == 0)), METHOD_AF_SEND_DATA);
    RETURN_INVALID_CLUSTER_IF_TRUE( (clusterId == 0), METHOD_AF_SEND_DATA);
    
#ifdef AF_VERBOSE     
    printf("Sending %u bytes to endpoint %u from endpoint %u with cluster %u (%04X) at Short Address %u (0x%04X)\r\n", 
           dataLength, destinationEndpoint, sourceEndpoint, clusterId, clusterId, destinationShortAddress, destinationShortAddress);
#endif  
    
#define AF_DATA_REQUEST_PAYLOAD_LEN 10
    
    zmBuf[0] = AF_DATA_REQUEST_PAYLOAD_LEN + dataLength;
    zmBuf[1] = MSB(AF_DATA_REQUEST);
    zmBuf[2] = LSB(AF_DATA_REQUEST);      
    
    zmBuf[3] = LSB(destinationShortAddress); 
    zmBuf[4] = MSB(destinationShortAddress);
    zmBuf[5] = destinationEndpoint;
    zmBuf[6] = sourceEndpoint;
    zmBuf[7] = LSB(clusterId); 
    zmBuf[8] = MSB(clusterId); 
    zmBuf[9] = transactionSequenceNumber;  //Improperly read on Stellaris when post-increment operation here
    zmBuf[10] = acknowledgmentMode;
    zmBuf[11] = DEFAULT_RADIUS;
    zmBuf[12] = dataLength; 
    transactionSequenceNumber++;
    
    memcpy(zmBuf+AF_DATA_REQUEST_PAYLOAD_LEN+3, data, dataLength);
    RETURN_RESULT_IF_FAIL(sendMessage(), METHOD_AF_SEND_DATA); 
#define AF_DATA_REQUEST_SRSP_STATUS_FIELD   SRSP_PAYLOAD_START
    //Now check the status returned in the SRSP:
    
#ifdef AF_DATA_CONFIRM_HANDLED_BY_APPLICATION           //Return control to main application
    RETURN_RESULT(zmBuf[AF_DATA_REQUEST_SRSP_STATUS_FIELD], METHOD_AF_SEND_DATA); 
#else
    RETURN_RESULT_IF_FAIL(zmBuf[AF_DATA_REQUEST_SRSP_STATUS_FIELD], METHOD_AF_SEND_DATA); 
    
#define AF_DATA_CONFIRM_TIMEOUT 2
    RETURN_RESULT_IF_FAIL(waitForMessage(AF_DATA_CONFIRM, AF_DATA_CONFIRM_TIMEOUT), METHOD_AF_SEND_DATA);
    RETURN_RESULT(zmBuf[AF_DATA_CONFIRM_STATUS_FIELD], METHOD_AF_SEND_DATA);  
#endif
}


#define METHOD_AF_DATA_STORE                    0x2400
/** Upload a chunk of data to the Module. Private helper method for afSendDataExtended().
 * @param index where in the whole message this chunk of bytes should start
 * @param data the bytes to store
 * @param dataLength how many bytes to store. A length of zero is special and triggers the actually
 * sending of the data request over the air.
*/
static moduleResult_t afDataStore(uint16_t index, uint8_t* data, uint8_t dataLength)
{
#define MAXIMUM_DATA_STORE_PAYLOAD_LENGTH 247    
    RETURN_INVALID_LENGTH_IF_TRUE( (dataLength > MAXIMUM_DATA_STORE_PAYLOAD_LENGTH), METHOD_AF_DATA_STORE);
    
#ifdef AF_VERBOSE     
    printf("Storing %u bytes starting at index %u\r\n", dataLength, index);
#endif 
#define AF_DATA_STORE_HEADER_LEN           3
    zmBuf[0] = AF_DATA_STORE_HEADER_LEN + dataLength;
    zmBuf[1] = MSB(AF_DATA_STORE);
    zmBuf[2] = LSB(AF_DATA_STORE);  
    
    zmBuf[3] = LSB(index); 
    zmBuf[4] = MSB(index);
    zmBuf[5] = dataLength;
    memcpy(zmBuf+AF_DATA_STORE_HEADER_LEN + 3, data, dataLength);
    RETURN_RESULT(sendMessage(), METHOD_AF_DATA_STORE);
}

//Note: no method ID since this is a simple wrapper method, and wrapped method does all error checking
/** Simple wrapper function to send extended messages via short address.
@see afSendData for description of these fields.
*/
moduleResult_t afSendDataExtendedShort(uint8_t _destinationEndpoint, uint8_t _sourceEndpoint,
                                       uint16_t _destinationShortAddress, 
                                       uint16_t _clusterId, uint8_t* _data, uint16_t _dataLength)
{
#ifdef AF_VERBOSE     
    printf("Sending EXT-SHORT %u bytes to endpoint %u from endpoint %u with cluster %u (%04X) at Short Address %u (0x%04X)\r\n", 
           _dataLength, _destinationEndpoint, _sourceEndpoint, _clusterId, _clusterId, _destinationShortAddress, _destinationShortAddress);
#endif  
    
    uint8_t address[8];
    address[0] = LSB(_destinationShortAddress);
    address[1] = MSB(_destinationShortAddress);
    return afSendDataExtended(_destinationEndpoint, _sourceEndpoint, address, DESTINATION_ADDRESS_MODE_SHORT,
                              _clusterId, _data, _dataLength);
}

#define METHOD_AF_DATA_REQUEST_EXT                    0x2600
/** Sends a message using extended messaging. This is more flexible and allows for long addressing.
@param destinationLongAddress If using short addressing then the first two bytes are the short address, 
LSB first. Remaining 6 bytes are don't care. If using long addressing then this is the 8 byte MAC, and LSB first.
@param destinationAddressMode Either DESTINATION_ADDRESS_MODE_LONG or DESTINATION_ADDRESS_MODE_SHORT
@see afSendData for description of remaining fields.
*/
moduleResult_t afSendDataExtended(uint8_t destinationEndpoint, uint8_t sourceEndpoint,
                                  uint8_t* destinationAddress, uint8_t destinationAddressMode,
                                  uint16_t clusterId, uint8_t* data, uint16_t dataLength)
{
    RETURN_INVALID_LENGTH_IF_TRUE( ((dataLength > AF_DATA_REQUEST_EXT_MAX_TOTAL_PAYLOAD_LENGTH) || (dataLength == 0)), METHOD_AF_DATA_REQUEST_EXT);
    RETURN_INVALID_CLUSTER_IF_TRUE( (clusterId == 0), METHOD_AF_DATA_REQUEST_EXT);
	RETURN_INVALID_PARAMETER_IF_TRUE( (destinationAddressMode > DESTINATION_ADDRESS_MODE_LONG) && (destinationAddressMode !=DESTINATION_ADDRESS_MODE_BROADCAST), METHOD_AF_DATA_REQUEST_EXT);
    
#ifdef AF_VERBOSE     
    /*char* destinationAddressModeName = (destinationAddressMode == DESTINATION_ADDRESS_MODE_LONG) ? "LONG" : "SHORT";
    printf("Sending EXT %u bytes to endpoint %u from endpoint %u with cluster %u (%04X) using addressing mode %u (%s) at Address ",
           dataLength, destinationEndpoint, sourceEndpoint, clusterId, clusterId, destinationAddressMode, destinationAddressModeName);
    printHexBytes(destinationAddress, 8);*/
#endif  
#define AF_DATA_REQUEST_EXT_HEADER_LEN  20
    //zmBuf[0] = AF_DATA_REQUEST_EXT_HEADER_LEN + dataLength;
    // Note: zmBuf[0] (length of this message) will be set below, based on whether the payload will fit in one message.
    zmBuf[1] = MSB(AF_DATA_REQUEST_EXT);
    zmBuf[2] = LSB(AF_DATA_REQUEST_EXT);      
    zmBuf[3] = destinationAddressMode;
    if (destinationAddressMode == DESTINATION_ADDRESS_MODE_LONG) 
    {
        memcpy(zmBuf+4, destinationAddress, 8);
    } else {  // short addressing
        memcpy(zmBuf+4, destinationAddress, 2);  //remaining bytes are don't care
    } 
    zmBuf[12] = destinationEndpoint;
    zmBuf[13] = LSB(INTRA_PAN);
    zmBuf[14] = MSB(INTRA_PAN);
    zmBuf[15] = sourceEndpoint;
    zmBuf[16] = LSB(clusterId); 
    zmBuf[17] = MSB(clusterId); 
    zmBuf[18] = transactionSequenceNumber;  //this value will get returned for use by higher level
    zmBuf[19] = acknowledgmentMode;
    zmBuf[20] = DEFAULT_RADIUS;
    zmBuf[21] = LSB(dataLength); 
    zmBuf[22] = MSB(dataLength); 
    transactionSequenceNumber++;

#define AF_DATA_REQUEST_EXT_SRSP_STATUS_FIELD   SRSP_PAYLOAD_START
    
    /* if payload IS short enough, then include it in this message and send the message just like sendMessage() */
    if (dataLength <= AF_DATA_REQUEST_EXT_MAX_PAYLOAD_LENGTH)                       
    {
        zmBuf[0] = AF_DATA_REQUEST_EXT_HEADER_LEN + dataLength;        
#ifdef AF_VERBOSE
        printf("Sending all in one message since dataLength %u < AF_DATA_REQUEST_EXT_MAX_PAYLOAD_LENGTH %u\r\n", dataLength, AF_DATA_REQUEST_EXT_MAX_PAYLOAD_LENGTH);
#endif
        memcpy(zmBuf+AF_DATA_REQUEST_EXT_HEADER_LEN+3, data, dataLength);
        
#ifdef AF_DATA_CONFIRM_HANDLED_BY_APPLICATION           //Return control to main application
        RETURN_RESULT_IF_FAIL(sendMessage(), METHOD_AF_DATA_REQUEST_EXT);         
        RETURN_RESULT(zmBuf[AF_DATA_REQUEST_EXT_SRSP_STATUS_FIELD], METHOD_AF_DATA_REQUEST_EXT);          
#else
        RETURN_RESULT_IF_FAIL(sendMessage(), METHOD_AF_DATA_REQUEST_EXT); 
        RETURN_RESULT_IF_FAIL(zmBuf[AF_DATA_REQUEST_EXT_SRSP_STATUS_FIELD], METHOD_AF_DATA_REQUEST_EXT);       
        
#define AF_DATA_CONFIRM_TIMEOUT 2
        RETURN_RESULT_IF_FAIL(waitForMessage(AF_DATA_CONFIRM, AF_DATA_CONFIRM_TIMEOUT), METHOD_AF_DATA_REQUEST_EXT);
        RETURN_RESULT(zmBuf[AF_DATA_CONFIRM_STATUS_FIELD], METHOD_AF_DATA_REQUEST_EXT);              
        
#endif
        //all done!
    } else {
        zmBuf[0] = AF_DATA_REQUEST_EXT_HEADER_LEN; // no payload included in this message.
        /* The message was larger than could fit into one message, so send the AF_DATA_REQUEST_EXT and then store the data */
        RETURN_RESULT_IF_FAIL(sendMessage(), METHOD_AF_DATA_REQUEST_EXT);      
        RETURN_RESULT_IF_FAIL(zmBuf[AF_DATA_REQUEST_EXT_SRSP_STATUS_FIELD], METHOD_AF_DATA_REQUEST_EXT);  
        
        /** Index in the Module data buffer. This will be sent to the Module */
        uint16_t totalMessageIndex = 0;                          
        
        while (dataLength > 0)                                   //while there is still more data to be stored...
        {
            /** How many bytes to send in this afDataStore message */
            uint8_t bytesToSend = 0;
            
            if (dataLength > MAXIMUM_DATA_STORE_PAYLOAD_LENGTH)  //if more bytes than what will fit in one message
            {
                bytesToSend = MAXIMUM_DATA_STORE_PAYLOAD_LENGTH; // then only send MAXIMUM_DATA_STORE_PAYLOAD_LENGTH bytes
            } else {
                bytesToSend = dataLength;                        // otherwise it will all fit in one afDataStore message
            }

            RETURN_RESULT_IF_FAIL(afDataStore(totalMessageIndex, data, bytesToSend), METHOD_AF_DATA_REQUEST_EXT);  //store each chunk of the total message
            
            dataLength -= bytesToSend;                           // decrement our byte counter
            totalMessageIndex += bytesToSend;       
#ifdef AF_VERBOSE  
            printf("Sent %u Bytes, %u remaining\r\n", bytesToSend, dataLength);
#endif
        }
        
#ifdef AF_DATA_CONFIRM_HANDLED_BY_APPLICATION
        RETURN_RESULT(afDataStore(0, data, 0), METHOD_AF_DATA_REQUEST_EXT);  
#else
        /* Now we send a final afDataStore with length of 0 to indicate that we're done sending data */
        RETURN_RESULT_IF_FAIL(afDataStore(0, data, 0), METHOD_AF_DATA_REQUEST_EXT);
        RETURN_RESULT(waitForMessage(AF_DATA_CONFIRM, AF_DATA_CONFIRM_TIMEOUT), METHOD_AF_DATA_REQUEST_EXT);
#endif
    }
}

#define METHOD_AF_DATA_RETRIEVE                    0x2700
/** Private method used to retrieve bytes from the Module message store.
@pre a AF_INCOMING_MSG_EXT was received
@param timestamp the timestamp of the AF_INCOMING_MSG_EXT message, to identify the message
@param index where to start the retrieval inside the Module's message store
@param dataLength how many bytes to retrieve. Must be less than MAXIMUM_AF_DATA_RETRIEVE_PAYLOAD_LENGTH.
A length of zero is special and triggers the freeing of the corresponding incoming message
@return MODULE_SUCCESS, or error code
*/
static moduleResult_t afDataRetrieve(uint8_t* timestamp, uint16_t index, uint8_t dataLength)
{
#ifdef AF_VERBOSE     
    printf("afDataRetrieve: index=%u, dataLength=%u, timestamp= ", index, dataLength);
    printHexBytes(timestamp, 4);
#endif
    RETURN_INVALID_LENGTH_IF_TRUE( (dataLength > MAXIMUM_AF_DATA_RETRIEVE_PAYLOAD_LENGTH), METHOD_AF_DATA_RETRIEVE);
    
#define AF_DATA_RETRIEVE_PAYLOAD_LEN                7    
    zmBuf[0] = AF_DATA_RETRIEVE_PAYLOAD_LEN;
    zmBuf[1] = MSB(AF_DATA_RETRIEVE);
    zmBuf[2] = LSB(AF_DATA_RETRIEVE);      
    
    memcpy(zmBuf+3, timestamp, 4);
    zmBuf[7] = LSB(index);
    zmBuf[8] = MSB(index);
    zmBuf[9] = dataLength;
    RETURN_RESULT_IF_FAIL(sendMessage(), METHOD_AF_DATA_RETRIEVE); 
    RETURN_RESULT(zmBuf[AF_DATA_RETRIEVE_SRSP_STATUS_FIELD], METHOD_AF_DATA_RETRIEVE);
}

#define METHOD_AF_RETRIEVE_EXTENDED_MESSAGE                    0x2800
/** Retrieves all bytes of an AF_INCOMING_MESSAGE_EXT using multiple calls to AF_DATA_RETRIEVE as necessary.
@pre destinationPtr points to memory large enough to contain the entire message (500B?)
@param ts the timestamp of the message to retrieve.
@param destinationPtr where to copy the bytes
@param length of message payload to retrieve. Must be less than AF_DATA_REQUEST_EXT_MAX_TOTAL_PAYLOAD_LENGTH.
@post message will be copied into destinationPtr.
*/
moduleResult_t retrieveExtendedMessage(uint8_t* ts, uint16_t length, uint8_t* destinationPtr)
{
    RETURN_INVALID_LENGTH_IF_TRUE( ((length > AF_DATA_REQUEST_EXT_MAX_TOTAL_PAYLOAD_LENGTH) ||
                                    (length == 0)), METHOD_AF_RETRIEVE_EXTENDED_MESSAGE);    
    
#ifdef AF_VERBOSE    
    printf("Getting Extended Message, L%u, timestamp = ", length);
    printHexBytes(ts, 4);
#endif
    
    /* Index in the destinationPtr buffer containing the bytes we're writing. Will be sent to the Module */
    uint16_t totalMessageIndex = 0;
    
    /* This identifies which message we are retrieving. Basically a unique ID of the message */
    uint8_t timestamp[4];
    memcpy(timestamp, ts, 4);    
    
    while (length > 0)  //while there are remaining bytes to be retrieved
    {
        uint8_t bytesToGet = 0;
        if (length > MAXIMUM_AF_DATA_RETRIEVE_PAYLOAD_LENGTH)       // If the number of remaining bytes is more than what will fit in a message
        {
            bytesToGet = MAXIMUM_AF_DATA_RETRIEVE_PAYLOAD_LENGTH;   // then only get as many as will fit
        } else {
            bytesToGet = length;                                    // otherwise get all that are remaining
        }
#ifdef AF_VERBOSE          
        printf("bytesToGet=%u, totalMessageIndex=%u\r\n", bytesToGet, totalMessageIndex);
#endif 
        
        RETURN_RESULT_IF_FAIL(afDataRetrieve(timestamp, totalMessageIndex, bytesToGet), METHOD_AF_RETRIEVE_EXTENDED_MESSAGE);
        memcpy(destinationPtr + totalMessageIndex, zmBuf+AF_DATA_RETRIEVE_SRSP_PAYLOAD_START_FIELD, bytesToGet); //copy this message's payload to the destination
        totalMessageIndex += bytesToGet;
        length -= bytesToGet;
    }
    
    // Now send a final afDataRetrieve with index=0 and length=0 to free the message
    RETURN_RESULT(afDataRetrieve(timestamp, 0, 0), METHOD_AF_RETRIEVE_EXTENDED_MESSAGE);
}

/** Displays the header information in an AF_INCOMING_MSG.
@param srsp a pointer to the buffer containing the message
@return 0 if success, -1 if not a AF_INCOMING_MSG.
*/
int16_t printAfIncomingMsgHeader(uint8_t* srsp)
{
    if (CONVERT_TO_INT(srsp[SRSP_CMD_LSB_FIELD], srsp[SRSP_CMD_MSB_FIELD]) == AF_INCOMING_MSG)
    {
        /*
        printf("#%02u: Grp%04x Clus%04x, SrcAd%04x, SrcEnd%02x DestEnd%02x Bc%02x Lqi%02x Sec%02x Len%02u", 
               srsp[SRSP_HEADER_SIZE+15],
               CONVERT_TO_INT(srsp[SRSP_HEADER_SIZE], srsp[SRSP_HEADER_SIZE+1]), 
               CONVERT_TO_INT(srsp[SRSP_HEADER_SIZE+2], srsp[SRSP_HEADER_SIZE+3]),
               CONVERT_TO_INT(srsp[SRSP_HEADER_SIZE+4], srsp[SRSP_HEADER_SIZE+5]), 
               srsp[SRSP_HEADER_SIZE+6], srsp[SRSP_HEADER_SIZE+7], srsp[SRSP_HEADER_SIZE+8], 
               srsp[SRSP_HEADER_SIZE+9], srsp[SRSP_HEADER_SIZE+10], srsp[SRSP_HEADER_SIZE+16]);
        */
        /* Don't show transaction sequence number unless non-zero */
        if (srsp[AF_INCOMING_MESSAGE_TRANSACTION_SEQUENCE_FIELD] != 0)
        {
            printf("#%02u ", srsp[AF_INCOMING_MESSAGE_TRANSACTION_SEQUENCE_FIELD]);
        }
        /* Don't show group unless non-zero */        
        if (AF_INCOMING_MESSAGE_GROUP() != 0)
        {
            printf("Grp%04x ", AF_INCOMING_MESSAGE_GROUP());
        }
             
        printf("Clus%04x, SrcAd%04X, SrcEnd%02X DestEnd%02X Bc%02X Lqi%02X Sec%02X Len%02u", 
               AF_INCOMING_MESSAGE_CLUSTER(),
               AF_INCOMING_MESSAGE_SHORT_ADDRESS(), 
               srsp[AF_INCOMING_MESSAGE_SOURCE_EP_FIELD], 
               srsp[AF_INCOMING_MESSAGE_DESTINATION_EP_FIELD], 
               srsp[AF_INCOMING_MESSAGE_WAS_BROADCAST_FIELD], 
               srsp[AF_INCOMING_MESSAGE_LQI_FIELD], 
               srsp[AF_INCOMING_MESSAGE_SECURITY_USE_FIELD], 
               srsp[AF_INCOMING_MESSAGE_PAYLOAD_LEN_FIELD]);
        return 0;
    } else {
        return -1;
    }
}

/** Displays the names of the fields that correspond with printAfIncomingMsgHeader*/
void printAfIncomingMsgHeaderNames()
{
    printf("Fields are: Group, Cluster, Source Address, Source Endpoint, \
        Destination Endpoint, wasBroadcast, Link Quality Indicator, SecurityUse, Payload Length\r\n");    
}
