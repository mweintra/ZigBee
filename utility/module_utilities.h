/**
*  @file module_utilities.h
*
*  @brief  public methods for module_utilities.c
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

#ifndef MODULE_UTILITIES_H
#define MODULE_UTILITIES_H

#include "application_configuration.h"
#include "module.h"
#include <stddef.h>

/** 
This is a set of application configuration parameters. These parameters affect the way our
particular Zigbee Application will behave. */
struct moduleConfiguration
{
    /** Operating Region of the module*/
    uint16_t operatingRegion;
	
    /** The type of device we're starting, e.g. ROUTER.*/
    uint8_t deviceType;
    
    /** 
    The channelMask we'd like to use. Parameter must be one or more channels "OR"d together,
    e.g. (CHANNEL_MASK_13 | CHANNEL_MASK_17 | CHANNEL_MASK_21) for channels 13,17,21. May also
	be set to ANY_CHANNEL_MASK which will use all channels. */
    uint32_t channelMask;
    
    /** If you want to use a specific PAN_ID. Otherwise set to ANY_PAN. */
    uint16_t panId;
    
    /** Only used if this is an end device. How often the end device polls its associated router for data */
    uint16_t endDevicePollRate;
    
    /** Whether to erase the previous module configuration on startup. */
    uint8_t startupOptions;
    
    /** 
    What type of security to use, if any. There are three options:
    - SECURITY_MODE_PRECONFIGURED_KEYS - all devices must have the same 16B security key
    - SECURITY_MODE_COORD_DIST_KEYS - the coordinator will give all devices the key on join
    - SECURITY_MODE_OFF note: the module still uses encryption, just with its default key */
    uint8_t securityMode;
    
    /** 
    The security key to use if security is enabled. Only used if:
    - when deviceType is COORDINATOR: SECURITY_MODE_PRECONFIGURED_KEYS or SECURITY_MODE_COORD_DIST_KEYS
    - when deviceType is ROUTER: SECURITY_MODE_PRECONFIGURED_KEYS
    If key is not used then may be pointed to NULL.  */
    uint8_t* securityKey;
};

moduleResult_t startModule(const struct moduleConfiguration* mc, const struct applicationConfiguration* ac);
moduleResult_t expressStartModule(const struct moduleConfiguration* mc, const struct applicationConfiguration* ac);
uint8_t getDeviceStateForDeviceType(uint8_t deviceType);
//moduleResult_t expressStartModule(const struct moduleConfiguration* mc, const struct applicationConfiguration* ac);

#define DEFAULT_CHANNEL_MASK		(CHANNEL_MASK_11 | CHANNEL_MASK_14 | CHANNEL_MASK_17 | CHANNEL_MASK_20 | CHANNEL_MASK_23)
#define DEFAULT_POLL_RATE_MS		2000
#define DEFAULT_STARTUP_OPTIONS		(STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE)


extern uint8_t appCount;
extern const struct moduleConfiguration DEFAULT_MODULE_CONFIGURATION_COORDINATOR;
extern const struct moduleConfiguration DEFAULT_MODULE_CONFIGURATION_ROUTER;
extern const struct moduleConfiguration DEFAULT_MODULE_CONFIGURATION_END_DEVICE;

//void displayMessages();
void displayMessage();

#define GENERIC_APPLICATION_CONFIGURATION   0


/* Used in setModuleRfPower */
#define MODULE_REGION_NORTH_AMERICA         0x0000
#define MODULE_REGION_EUROPE                0x0001


#endif
