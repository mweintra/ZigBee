/**
* @file application_configuration.h
* 
*  @brief  public methods for application_configuration.c
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

#ifndef APPLICATION_CONFIGURATION_H
#define APPLICATION_CONFIGURATION_H
#include <stdint.h>

//default values used when creating applicationConfigurations in Simple API or AFZDO
#define DEFAULT_ENDPOINT        0xD7 
//#define DEFAULT_ENDPOINT 0xD6
#define DEFAULT_PROFILE_ID      0xD7D7
#define DEVICE_ID               0x4567
#define DEVICE_VERSION          0x89
#define DEFAULT_LATENCY			LATENCY_NORMAL
#define INFO_MESSAGE_CLUSTER  	0x07

//Values for latencyRequested field of struct applicationConfiguration. Not used in Simple API.
#define LATENCY_NORMAL          0
#define LATENCY_FAST_BEACONS    1
#define LATENCY_SLOW_BEACONS    2


/** This is a set of application configuration parameters. These parameters affect the way our 
particular Zigbee Application will behave. */
struct applicationConfiguration
{
  /** The Zigbee Endpoint. Simple API only supports one endpoint. AF supports multiple endpoints.
    Must use the same value of endpoint for all devices in the network.*/
  uint8_t endPoint;
  
  /** The Application Profile ID as assigned by the Zigbee Association. 
  Must use the same application profile for all devices in the network. */
  uint16_t profileId;
  
  /** A user-defined device ID field. Not used in Simple API.
  When using AFZDO API a remote device can query for this using the ZDO_SIMPLE_DESC_REQ command. */
  uint16_t deviceId;
  
  /** A user-defined device ID field. Not used in Simple API.
  When using AFZDO API a remote device can query for this using the ZDO_SIMPLE_DESC_REQ command. */ 
  uint8_t deviceVersion;
    
  /** A very simple Quality of Service (QoS) setting. Not used in Simple API.
  When using AFZDO API must be LATENCY_NORMAL, LATENCY_FAST_BEACONS, or LATENCY_SLOW_BEACONS.*/
  uint8_t latencyRequested;

#ifndef __MSP430G2553
	#define MAX_BINDING_CLUSTERS 4  /** Maximum number of clusters available for binding is actually higher (32) but we use a lower number to conserve memory*/
#else
	#define MAX_BINDING_CLUSTERS 1
#endif
  /** Number of Input Clusters for Binding. If not using binding then set to zero.*/
  uint8_t numberOfBindingInputClusters;

  /** List of Input Clusters for Binding. If not using binding then this does not apply. 
  To allow another device to bind to this device, must use ZB_ALLOW_BIND on this device and must also
  use ZB_BIND_DEVICE on the other device. */  

  uint16_t bindingInputClusters[MAX_BINDING_CLUSTERS];

  
  /** Number of Output Clusters for Binding. If not using binding then set to zero.*/  
  uint8_t numberOfBindingOutputClusters;

  /** List of Output Clusters for Binding. If not using binding then this does not apply. 
  To bind to another device, that device must use ZB_ALLOW_BIND and this device must use 
  ZB_BIND_DEVICE to create a binding. */    

  uint16_t bindingOutputClusters[MAX_BINDING_CLUSTERS];


  
};

#define MINIMUM_ZIGBEE_ENDPOINT			0x01
#define MAXIMUM_ZIGBEE_ENDPOINT			0xF0
#define IS_VALID_ZIGBEE_ENDPOINT(ep)	((ep >= MINIMUM_ZIGBEE_ENDPOINT) && (ep <= MAXIMUM_ZIGBEE_ENDPOINT))

void displayApplicationConfiguration(const struct applicationConfiguration* ac);

extern const struct applicationConfiguration DEFAULT_APPLICATION_CONFIGURATION;
#endif
