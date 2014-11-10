/**
* @file application_configuration.c
*
* @brief Contains Zigbee Application Configuration methods. 
*
* An Application Configuration defines how the Module will function on the Zigbee Network. This
* includes which profile is used, which endpoints are used, whether you want binding, etc.
* This is used by both Simple API and AFZDO to configure the Module for the particular application.
* Simple API can register one applicationConfiguration.
* AFZDO can register multiple applicationConfigurations, each having its own endpoint.
* Refer to documentation for more information.
* @note if using Simple API, the method sapiRegisterGenericApplication() does most of what you need.
* @note if using AF, the method afRegisterGenericApplication() does most of what you need.
* @note binding is unsupported.
* @note to configure a Coordinator with one binding input cluster 0x0001, then configure the ac with:
 -  ac.numberOfBindingInputClusters =   1; 
 -  ac.bindingInputClusters[0] =        0x0001;    
 -  ac.numberOfBindingOutputClusters =  0;   
*
* @note to configure a Router with one binding output cluster 0x0001, then configure the ac with:
 -  ac.numberOfBindingInputClusters =   0;   
 -  ac.numberOfBindingOutputClusters =  1;
 -  ac.bindingOutputClusters[0] =       0x0001;
*
*
* @see sapiRegisterApplication() and sapiRegisterGenericApplication()
* @see afRegisterApplication() and afRegisterGenericApplication()
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

#include "hal.h"
#include "application_configuration.h"  
#include <stdint.h>

/** Displays the applicationConfiguration in a human readable format
@param ac the applicationConfiguration to display
@see applicationConfiguration 
*/

const struct applicationConfiguration DEFAULT_APPLICATION_CONFIGURATION = {
DEFAULT_ENDPOINT,
DEFAULT_PROFILE_ID,
DEVICE_ID,
DEVICE_VERSION,
DEFAULT_LATENCY,
0,
NULL,
0,
NULL
};

void displayApplicationConfiguration(const struct applicationConfiguration* ac)
{
    printf("Application Configuration:\r\n");    
    printf("    Endpoint = 0x%02X\r\n", ac->endPoint);
    printf("    ProfileId = 0x%04X\r\n", ac->profileId);
    printf("    DeviceId = 0x%04X\r\n", ac->deviceId);
    printf("    DeviceVersion = 0x%02X\r\n", ac->deviceVersion);
    printf("    LatencyRequested = ");
    switch (ac->latencyRequested)
    {
    case LATENCY_NORMAL: 
        printf("NORMAL");
    break;
    case LATENCY_FAST_BEACONS: 
        printf("FAST_BEACONS"); 
    break;
    case LATENCY_SLOW_BEACONS: 
        printf("SLOW_BEACONS"); 
    break;
    default: 
        printf("UNKNOWN"); 
    break;
    }
        printf("\n\r");
    printf("    Number of Binding Input Clusters = %u ", ac->numberOfBindingInputClusters);
    if (ac->numberOfBindingInputClusters > 0)
    {
        printf("{");
        int i;
        for (i=0; i< ac->numberOfBindingInputClusters; i++)
            printf("0x%02X ", ac->bindingInputClusters[i]);
        printf("}");
    }
        printf("\n\r");
    printf("    Number of Binding Output Clusters = %u ", ac->numberOfBindingOutputClusters);
    if (ac->numberOfBindingOutputClusters > 0)
    {
        printf("{");
        int i;
        for (i=0; i< ac->numberOfBindingOutputClusters; i++)
            printf("0x%02X ", ac->bindingOutputClusters[i]);
        printf("}");
    }
        printf("\n\r");
}
