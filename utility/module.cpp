/**
* @file module.c
*
* @brief Interface to the Zigbee Module
* 
* This file contains library methods as well as error checking and return values.
*
* Requires one physical interface to be defined, e.g. ZM_PHY_SPI.
*
* @note To output SPI transmit information, define MODULE_INTERFACE_VERBOSE. 
* It is recommended to define this on a per-project basis. 
* In IAR, this can be done in Project Options : C/C++ compiler : Preprocessor
* In the defined symbols box, add:
* MODULE_INTERFACE_VERBOSE
*
* $Rev: 1969 $
* $Author: dsmith $
* $Date: 2013-11-25 17:16:29 -0800 (Mon, 25 Nov 2013) $
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

#include "module.h"
#include <string.h>                     //for memcpy
#include "hal.h"
#include "utilities.h"
#include "zm_phy_spi.h"
#include "module_errors.h"
#include <stddef.h>                     //for NULL
#include <stdint.h>

//#define MODULE_INTERFACE_VERBOSE
//#define ZM_INTERFACE_VERBOSE

extern uint8_t zmBuf[ZIGBEE_MODULE_BUFFER_SIZE];

/** 
Initializes the module's physical interface, either SPI or UART, depending on which phy file is included.
*/
void moduleInit()
{
  zm_phy_init();  //this is phy dependent.  
}

#define METHOD_MODULE_RESET        0x0100
/** 
Resets the Module using hardware and retrieves the SYS_RESET_IND message. This method is used to 
restart the Module's internal state machine and apply changes to startup options, zigbee device type, etc.
@post zmBuf contains the version structure, starting at MODULE_RESET_RESULT_START_FIELD
@see Interface Specification for order of fields
*/
moduleResult_t moduleReset()
{
    RADIO_OFF();
    delayMs(1);
    RADIO_ON(); 
#define MODULE_RESET_INITIAL_DELAY_MS   400
    delayMs(MODULE_RESET_INITIAL_DELAY_MS);                        //Necessary to allow proper module startup

#define TEST_SRDY_INTERVAL_MS           10        // check SRDY every 10 mSec
#define TEST_SRDY_TIMEOUT_MS            2000      // ...and timeout after 1500mSec
#define TEST_SRDY_MINIMUM_TIMEOUT_MS    100       // If SRDY transitions less then this than an error.
    unsigned int elapsedTime = 0;       //now, poll for SRDY going low...

    do
    {
        delayMs(TEST_SRDY_INTERVAL_MS);
        elapsedTime += TEST_SRDY_INTERVAL_MS;
    }
    while ((elapsedTime < TEST_SRDY_TIMEOUT_MS) && (!(MODULE_HAS_MESSAGE_WAITING())));

    RETURN_RESULT_IF_EXPRESSION_TRUE(((SRDY_IS_HIGH()) || (elapsedTime < TEST_SRDY_MINIMUM_TIMEOUT_MS)), METHOD_MODULE_RESET, TIMEOUT);

#ifdef MODULE_INTERFACE_VERBOSE
    printf("Module ready in %umS\r\n", elapsedTime + MODULE_RESET_INITIAL_DELAY_MS);
#endif

    return (getMessage());


}

/** 
Displays the contents of a SYS_RESET_IND message. These are returned from the module after a 
hard or soft reset. 
@pre zmBuf contains a valid SYS_RESET_IND message. 
*/
void displaySysResetInd()
{
    if (IS_SYS_RESET_IND())
    {
        uint8_t* v = zmBuf + SYS_RESET_IND_START_FIELD;
        printf("%s (%u), Transport=%u, ProductId=0x%02X, FW Rev=%u.%u.%u\r\n",
               getResetReason(v[0]), v[0], v[1], v[2], v[3], v[4], v[5]);
    } else {
        printf("Error - not a SYS_RESET_IND. Expected type 0x%04X; Received type 0x%04X; Contents:\r\n", SYS_RESET_IND, MODULE_COMMAND());
        displayZmBuf(); 
    }
}

/** 
Utility method for displaying the reset reason from a SYS_RESET_IND message.
@param reason the reset reason
@return A string containing the name of the reset reason, or "Unknown" if it is not known.
*/
char* getResetReason(uint8_t reason)
{
    switch (reason)
    {
    case 0:     return "Power-up";
    case 1:     return "External";
    case 2:     return "Watch-dog";
    default:    return "Unknown";
    }
}

#define METHOD_SYS_VERSION	0x0200
/** 
Retrieves the Module's version information using SYS_VERSION command.
@post zmBuf contains the version structure, starting at SYS_VERSION_RESULT_START_FIELD
*/
moduleResult_t sysVersion()
{
#define SYS_VERSION_PAYLOAD_LEN 0
    zmBuf[0] = SYS_VERSION_PAYLOAD_LEN;
    zmBuf[1] = MSB(SYS_VERSION);
    zmBuf[2] = LSB(SYS_VERSION);   
    RETURN_RESULT(sendMessage(), METHOD_SYS_VERSION);     
}

/** 
Displays a SYS_VERSION message or an error message if the message in zmBuf is not a SYS_VERSION.
The SYS_VERSION message is returned by the module after a call to sysVersion().
@pre zmBuf contains a valid SYS_VERSION message 
*/
void displaySysVersion()
{
    if (IS_SYS_VERSION_SRSP())
    {
        /* Display the information in the SYS_VERSION message */
        printf("Module Version: Transport=%u, ProductId=0x%02X, FW Rev=%u.%u.%u\r\n",
               zmBuf[SYS_VERSION_RESULT_START_FIELD], zmBuf[SYS_VERSION_RESULT_PRODUCTID_FIELD], zmBuf[SYS_VERSION_RESULT_FW_MAJOR_FIELD], zmBuf[SYS_VERSION_RESULT_FW_MINOR_FIELD], zmBuf[SYS_VERSION_RESULT_FW_BUILD_FIELD]);    
    } else {
        /* It's not what we want, so display the message type of the received message vs. what we expect */
        printf("Error - not a SYS_VERSION SRSP. Expected type 0x%04X; Received type 0x%04X; Contents:\r\n", (SYS_VERSION + SRSP), MODULE_COMMAND());
        displayZmBuf();
    }
}

#define METHOD_SYS_RANDOM	0x0300
/** 
Retrieves a random number from the Module using SYS_RANDOM command. The Module has a hardware random
number generator that uses RF noise to derive the random number.
@post The random number is contained in zmBuf[SYS_RANDOM_RESULT_LSB_FIELD] and zmBuf[SYS_RANDOM_RESULT_MSB_FIELD].
@note You can also use utility macro SYS_RANDOM_RESULT() to extract the random number from zmBuf.
*/
moduleResult_t sysRandom()
{
#define SYS_RANDOM_PAYLOAD_LEN 0
    zmBuf[0] = SYS_RANDOM_PAYLOAD_LEN;
    zmBuf[1] = MSB(SYS_RANDOM);
    zmBuf[2] = LSB(SYS_RANDOM);  
    RETURN_RESULT(sendMessage(), METHOD_SYS_RANDOM);    
}

#define METHOD_SYS_SET_TX_POWER                    0x1100  // Out of sequence because it was added later
/** 
Sets the Tx power of the module.
@param txPowerSetting the desired RF Tx setting. This modifies the CC2530's internal PA. Refer to module 
documentation for information about which setting corresponds to which RF output power. This will
vary based on the type of module used (range extender vs. no range extender).
@param actualTxPowerSetting the actual output power setting that the module applies. It will be the
same as txPowerSetting unless that value was not valid.
@post The module's transmit power has been set accordingly.
@warning Improper use of this command may invalidate FCC/ETSI certification. Use with caution.
@since Module Firmware version 2.5.1. This function will not work on 2.4.0 firmware; instead you
will receive a SRSP of 0x6000 (error)
@note this method modifies the Tx power of the IC. The emitted RF power will be slightly less due
to matching network, balun (if not a module with range-extender), and antenna. See documentation.
*/
moduleResult_t sysSetTxPower(uint8_t txPowerSetting, uint8_t* actualTxPowerSetting)
{
#ifdef MODULE_INTERFACE_VERBOSE     
    printf("Setting TX_POWER to %u requested; ", txPowerSetting);
#endif
#define SYS_SET_TX_POWER_PAYLOAD_LEN 1
    zmBuf[0] = SYS_SET_TX_POWER_PAYLOAD_LEN;
    zmBuf[1] = MSB(SYS_SET_TX_POWER);
    zmBuf[2] = LSB(SYS_SET_TX_POWER);
    
    zmBuf[3] = txPowerSetting;
    RETURN_RESULT_IF_FAIL(sendMessage(), METHOD_SYS_SET_TX_POWER); 
#ifdef MODULE_INTERFACE_VERBOSE     
    printf("Actual TX_POWER set to %d\r\n", zmBuf[SYS_SET_TX_POWER_RESULT_FIELD]);
#endif
    *actualTxPowerSetting = zmBuf[SYS_SET_TX_POWER_RESULT_FIELD];
   return MODULE_SUCCESS;
}


//note: no method ID for this one; it should be wrapped by others.
/** 
Private utility method to write configuration data to the Module
@param zcd the Zigbee configuration data ID
@param zcdLength the length of above
@param data an array containing the data to write.
@note All ZB_WRITE_CONFIGURATION commands take approx. 3.5mSec between SREQ & SRSP; presumably to 
write to flash inside the Module. 
*/
moduleResult_t zbWriteConfiguration(uint8_t zcd, uint8_t zcdLength, uint8_t* data)
{
#define ZB_WRITE_CONFIGURATION_LEN      2  //excluding payload length
    zmBuf[0] = ZB_WRITE_CONFIGURATION_LEN + zcdLength;
    zmBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    zmBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
    
    zmBuf[3] = zcd;
    zmBuf[4] = zcdLength;
    memcpy(zmBuf+5, data, zcdLength);
    return (sendMessage()); 
}


#define METHOD_SET_PAN_ID	0x0400
/** 
Configures the Module to only join a network with the given panId. If panId = ANY_PAN (the default)
then the module will join any network.
@param panId the PANID to join, or ANY_PAN to join any PAN
@note be sure you know what you're doing if you're restricting your network to a specific PANID. In
normal operation this command should not be used, or else you may end up with two networks with the 
same PANID, which is bad.
*/
moduleResult_t setPanId(uint16_t panId)
{
#ifdef MODULE_INTERFACE_VERBOSE 
    printf("Setting Zigbee PAN ID to %04X\r\n", panId);
#endif     
    uint8_t data[2];
    data[0] = LSB(panId);
    data[1] = MSB(panId);
    RETURN_RESULT((zbWriteConfiguration(ZCD_NV_PANID, ZCD_NV_PANID_LEN, data)), METHOD_SET_PAN_ID);
}


#define METHOD_SET_CHANNEL	0x0500
/** 
Configures the Module to only join the specified channel. Simple wrapper method for setChannelMask 
if only one channel is desired. Overwrites the current channel mask.
@param channel must be 11..25, inclusive. If set to 0 then Module will scan ALL CHANNELS. If channel
is not 11..25 or 0 then the error INVALID_PARAMETER will be returned.
@note If channel or channel mask is NOT set then Module will use channel 11 only.
@note Channel 26 can only be used if INCLUDE_CHANNEL_26 in module.h is defined.
@see setChannelMask()
*/
moduleResult_t setChannel(uint8_t channel)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( ((!IS_VALID_CHANNEL(channel)) && (channel != 0x00)), METHOD_SET_CHANNEL);

    uint32_t channelMask = 1;
    return (setChannelMask((channel == 0) ? ANY_CHANNEL_MASK : (channelMask << channel)));
}


#define METHOD_SET_CHANNEL_MASK	        0x0600
/** 
Configures which RF channels the Module may use to join a network. The module will only use the
channels specified in the channel mask. Functionality depends on whether the device is a Coordinator 
or Router/End Device:
 - <i>Coordinator</i> The Module will only create a network on one of the channels that matches the 
mask. When the coordinator starts up, it scans all the channels in its channel mask and picks the 
channel with the least noise. The coordinator will stay on the same channel until it is reset. 
 - <i>Router or End Device</i> The Module will only search on channels that match the channel mask. 
The more channels in the channel mask, the longer startup will take since the device has to scan 
each channel.

@param channelMask bitmap of which channels to use. If channelMask = ANY_CHANNEL then the radio will 
join a network on any channel. Must be between MIN_CHANNEL_MASK and MAX_CHANNEL_MASK, or ANY_CHANNEL.
@note The default channel after a CLEAR_CONFIG is channel 11, or (MSB first) 0x00000800, or binary 1000|0000|0000
@note For example, to allow only channels 11, 14, and 15, then set channel mask 0x980,  binary 1001|1000|0000
@note if using a packet sniffer, be sure to change the channel on the packet sniffer too or else 
you won't see any packets.
@note Channel 26 cannot be used on modules with a range extender due to FCC limits on radiated power.
If you would like to use channel 26, then #define INCLUDE_CHANNEL_26 in module.h
*/
moduleResult_t setChannelMask(uint32_t channelMask)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( ((channelMask < MIN_CHANNEL_MASK) || (channelMask > MAX_CHANNEL_MASK)), METHOD_SET_CHANNEL_MASK);
    
    uint8_t data[4];
    data[0] = LSB(channelMask);
    data[1] = (channelMask & 0xFF00) >> 8;
    data[2] = (channelMask & 0xFF0000) >> 16;
    data[3] = channelMask >> 24;
#ifdef MODULE_INTERFACE_VERBOSE 
    printf("Setting to Channel List (LSB first): %02X %02X %02X %02X\r\n", data[0], data[1], data[2], data[3]);
#endif    
    RETURN_RESULT((zbWriteConfiguration(ZCD_NV_CHANLIST, ZCD_NV_CHANLIST_LEN, data)), METHOD_SET_CHANNEL_MASK);
}

#define METHOD_GET_DEVICE_INFO              0x0700
/** 
Retrieves the specified Device Information Property (DIP) from the Module.
@param dip the device information property to retrieve
@post zmBuf holds the requested DIP, LSB first, starting at SRSP_DIP_VALUE_FIELD
*/
moduleResult_t zbGetDeviceInfo(uint8_t dip)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( (dip > MAX_DEVICE_INFORMATION_PROPERTY), METHOD_GET_DEVICE_INFO);

#define ZB_GET_DEVICE_INFO_PAYLOAD_LEN 1
    zmBuf[0] = ZB_GET_DEVICE_INFO_PAYLOAD_LEN;
    zmBuf[1] = MSB(ZB_GET_DEVICE_INFO);
    zmBuf[2] = LSB(ZB_GET_DEVICE_INFO);
    zmBuf[3] = dip;
    RETURN_RESULT(sendMessage(), METHOD_GET_DEVICE_INFO); 
}


/*
*               NON-VOLATILE (NV) MEMORY ITEMS
*/

/** 
Retrieves the size of the selected Non-Volatile (NV) memory item. NV Items 1 through 4 are 2 bytes 
each; 5 and 6 are 16 bytes each. NV Items 8 & 9 are used internally by the module.
@param nvItem which Non-Volatile memory item to get the size of. Must be 1-6 inclusive.
@return The size of the given nvItem
*/
uint8_t getNvItemSize(uint8_t nvItem)
{
    if ((nvItem < 5) || (nvItem==8))
    {
        return 2;
    } else {
        return 16;
    }
}

#define METHOD_SYS_NV_READ              0x0800
/** 
Retrieves the specified Non-Volatile (NV) memory item from the Module. 
@pre Module was initialized.
@pre the nvItem was written to. If not then the NV item will contain indeterminate data.
@param nvItem which nvItem to write, 1 through 6 inclusive. If not one of these then INVALID_PARAMETER will be returned.
@post zmBuf contains the data read
@note does not verify the length of the returned nvItem was what we expected
*/
moduleResult_t sysNvRead(uint8_t nvItem)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( ((nvItem < MIN_NV_ITEM) || (nvItem > MAX_NV_ITEM) || (nvItem == NV_ITEM_RESERVED)), METHOD_SYS_NV_READ);
    
#define SYS_NV_READ_PAYLOAD_LEN 3  
    zmBuf[0] = SYS_NV_READ_PAYLOAD_LEN;
    zmBuf[1] = MSB(SYS_NV_READ);
    zmBuf[2] = LSB(SYS_NV_READ);  
    
    zmBuf[3] = nvItem;         //item number, 1-6
    zmBuf[4] = 0x0F;           //MSB of item number, but only items 1-6 are supported
    zmBuf[5] = 0;              //offset from beginning of the NV item, not used
    RETURN_RESULT_IF_FAIL(sendMessage(), METHOD_SYS_NV_READ); 
    RETURN_RESULT(zmBuf[SYS_NV_READ_STATUS_FIELD], METHOD_SYS_NV_READ);
}

#define METHOD_SYS_NV_WRITE              0x0900
/** 
Writes the specified Non-Volatile (NV) memory item to the Module. 
The contents of the selected nvItem will be overwritten from memory starting at data.
@pre Module was initialized.
@pre data points to the data to write into the non volatile memory item.
@param nvItem which nvItem to write. Items 1 through 6 are for user use; items 8 & 9 are for module
internal use. If not one of these then INVALID_PARAMETER will be returned.
@param data points to the data to be written
*/
moduleResult_t sysNvWrite(uint8_t nvItem, uint8_t* data)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( ((nvItem < MIN_NV_ITEM) || (nvItem > MAX_NV_ITEM) || (nvItem == NV_ITEM_RESERVED)), METHOD_SYS_NV_WRITE);
    
    uint8_t nvItemSize = getNvItemSize(nvItem);
#ifdef MODULE_INTERFACE_VERBOSE
    printf("Writing NV Item %u (length %u) with data: ", nvItem, nvItemSize);
    printHexBytes(data, nvItemSize);
#endif      
#define SYS_NV_WRITE_PAYLOAD_LEN 4  //excludes length of NV item    
    zmBuf[0] = SYS_NV_WRITE_PAYLOAD_LEN + nvItemSize;
    zmBuf[1] = MSB(SYS_NV_WRITE);
    zmBuf[2] = LSB(SYS_NV_WRITE);  
    
    zmBuf[3] = nvItem;         //item number, 1-6
    zmBuf[4] = 0x0F;           //MSB of item number, but only items 1-6 are supported
    zmBuf[5] = 0;              //offset from beginning of the NV item, not used
    zmBuf[6] = nvItemSize;     //length
    
    memcpy(zmBuf+7, data, nvItemSize);
    RETURN_RESULT(sendMessage(), METHOD_SYS_NV_WRITE);       
}

#define METHOD_SYS_GPIO              0x0A00
/** 
The Module has four GPIO pins, which can be configured as inputs or outputs. This command performs 
the specified General Purpose Input/Output (GPIO) operation.
@param operation which GPIO operation to do: GPIO_SET_DIRECTION, GPIO_SET_INPUT_MODE, GPIO_SET, 
GPIO_CLEAR, GPIO_TOGGLE, GPIO_READ
@param value the value to write to the outputs (if doing a GPIO_SET command)
@post zmBuf[SYS_GPIO_READ_RESULT_FIELD] contains the value returned by the Module if the operation 
was a GPIO_READ; indeterminate value otherwise
*/
moduleResult_t sysGpio(uint8_t operation, uint8_t value)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( ((operation > GPIO_OPERATION_MAX) || 
                                       ((value > 0x0F) && (operation != GPIO_SET_INPUT_MODE))), METHOD_SYS_GPIO);
    
#define SYS_GPIO_PAYLOAD_LEN 2
    zmBuf[0] = SYS_GPIO_PAYLOAD_LEN;
    zmBuf[1] = MSB(SYS_GPIO);
    zmBuf[2] = LSB(SYS_GPIO);
    
    zmBuf[3] = operation;
    zmBuf[4] = value;
    RETURN_RESULT(sendMessage(), METHOD_SYS_GPIO);        
}

#define METHOD_SET_RF_TEST_MODE              0x0B00
/** 
Configures the Module to output RF test tone. Send a system reset command after this command to 
enable. Must do a hard reset of Module to stop test tone.
@param mode The RF test mode; 1=Rx On, 2=UnModulated Tx, 3=Modulated Tx, 0=none
@param channel The channel to output test tone on; 11-26
@param txPower The RF output power of the CC2530. Actual module power output may be higher with PA.
@param txTone See CC2530 and CC2531 family guide, MDMTEST0 register description. Leave as 0.
@note Not all fields are validated.
@todo refactor with void sysNvWrite(uint8_t nvItem, uint8_t* data) ?
*/
moduleResult_t setRfTestMode(uint8_t mode, uint8_t channel, uint8_t txPower, uint8_t txTone)
{
#ifdef MODULE_INTERFACE_VERBOSE 
    printf("RF Test Mode %u, channel %u, txPower 0x%02X,  txTone %02X\r\n", mode, channel, txPower, txTone);
#endif    
    RETURN_INVALID_PARAMETER_IF_TRUE( ((!IS_VALID_CHANNEL(channel)) || (mode > RF_TEST_MODE_MAXIMUM)), METHOD_SET_RF_TEST_MODE);    
       
#define ZCD_NV_RF_TEST_MODE_LEN 4
    zmBuf[0] = SYS_NV_WRITE_PAYLOAD_LEN + ZCD_NV_RF_TEST_MODE_LEN;
    zmBuf[1] = MSB(SYS_NV_WRITE);
    zmBuf[2] = LSB(SYS_NV_WRITE);  
    
    zmBuf[3] = 0x07;           //LSB of item number
    zmBuf[4] = 0x0F;           //MSB of item number
    zmBuf[5] = 0;              //offset from beginning of the NV item, not used
    zmBuf[6] = ZCD_NV_RF_TEST_MODE_LEN;     //length
    zmBuf[7] = mode;
    zmBuf[8] = channel;
    zmBuf[9] = txPower;
    zmBuf[10] = txTone;
    RETURN_RESULT(sendMessage(), METHOD_SET_RF_TEST_MODE); 
}

/** 
Utility method for displaying the name of the RF Test Mode
@param mode the RF Test Mode
@return A string containing the name of the test mode, or "Unknown" if it is not known.
*/
char* getRfTestModeName(uint8_t mode)
{
    switch (mode)
    {
    case 0:                     return "None";
    case RF_TEST_MODULATED:     return "RF_TEST_MODULATED";
    case RF_TEST_UNMODULATED:   return "RF_TEST_UNMODULATED";
    case RF_TEST_RECEIVE:       return "RF_TEST_RECEIVE";
    default:                    return "Unknown";
    }
}

#define METHOD_SET_STARTUP_OPTIONS              0x0C00
/** 
Configures startup options on the Module. These will reset various parameters back to their 
factory defaults. The Module supports two types of clearing state, and both are supported:
- STARTOPT_CLEAR_CONFIG restores all settings to factory defaults. Must restart the Module after using this option.
- STARTOPT_CLEAR_STATE only clears network settings (PAN ID, channel, etc.)

@param option which options to set. Must be zero, STARTOPT_CLEAR_CONFIG, or STARTOPT_CLEAR_STATE. If
not one of these then an INVALID_PARAMETER error will be returned.
*/
moduleResult_t setStartupOptions(uint8_t option)
{ 
    RETURN_INVALID_PARAMETER_IF_TRUE( (option > (STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE)), METHOD_SET_STARTUP_OPTIONS);       
    
#ifdef MODULE_INTERFACE_VERBOSE     
    printf("Setting Startup Option to ");
    if (option & STARTOPT_CLEAR_CONFIG)
        printf("STARTOPT_CLEAR_CONFIG ");
    if (option & STARTOPT_CLEAR_STATE)
        printf("STARTOPT_CLEAR_STATE ");   
    printf("\n\r");
#endif
    uint8_t data[1];
    data[0] = option;
    RETURN_RESULT(zbWriteConfiguration(ZCD_NV_STARTUP_OPTION, ZCD_NV_STARTUP_OPTION_LEN, data), METHOD_SET_STARTUP_OPTIONS);     
}

/** 
Retrieves the name of the device type in a human-readable format, e.g. Coordinator, Router, or End Device. 
@param deviceType the type of device
@return The name of the deviceType
*/
char* getDeviceTypeName(uint8_t deviceType)
{
    switch (deviceType)
    {
    case COORDINATOR:               return("Coordinator");
    case ROUTER:                    return("Router");
    case END_DEVICE:                return("End Device");          
    default:                        return("Unknown");
    }
}

#define METHOD_SET_ZIGBEE_DEVICE_TYPE              0x0D00
/** 
Sets the Zigbee Device Type for the Module.
@param deviceType The type of Zigbee device. Must be COORDINATOR, ROUTER, or END_DEVICE 
*/
moduleResult_t setZigbeeDeviceType(uint8_t deviceType)
{
#ifdef ZM_INTERFACE_VERBOSE 
    printf("Setting Zigbee DeviceType to %s\r\n", getDeviceTypeName(deviceType));
#endif     
    RETURN_INVALID_PARAMETER_IF_TRUE( (deviceType > END_DEVICE), METHOD_SET_ZIGBEE_DEVICE_TYPE);      
    uint8_t data[1];
    data[0] = deviceType;
    RETURN_RESULT(zbWriteConfiguration(ZCD_NV_LOGICAL_TYPE, ZCD_NV_LOGICAL_TYPE_LEN, data), METHOD_SET_ZIGBEE_DEVICE_TYPE);
}

#define METHOD_SET_CALLBACKS                    0x0E00
/** 
Enable/Disabless callbacks on the Module. 
@param cb must be either CALLBACKS_ENABLED or CALLBACKS_DISABLED. If not one of these values then
the error INVALID_PARAMETER will be returned.
@see section ZCD_NV_ZDO_DIRECT_CB in Module Interface Specification
*/
moduleResult_t setCallbacks(uint8_t cb)
{ 
    RETURN_INVALID_PARAMETER_IF_TRUE( ((cb != CALLBACKS_ENABLED) && (cb != CALLBACKS_DISABLED)), METHOD_SET_CALLBACKS);      
    
#ifdef ZM_INTERFACE_VERBOSE     
    printf("Setting Callbacks to %s\r\n", (cb ? "ON" : "OFF"));
#endif
    uint8_t data[1];
    data[0] = cb;
    RETURN_RESULT(zbWriteConfiguration(ZCD_NV_ZDO_DIRECT_CB, ZCD_NV_ZDO_DIRECT_CB_LEN, data), METHOD_SET_CALLBACKS);
}


//
//      Miscellaneous Utilities
//
#define METHOD_WAIT_FOR_MESSAGE                    0x0F00
/** 
Waits for the specified type of message. Ignores any other messages received. The received message 
will be in zmBuf[]. If the specified type of message isn't received by timeout then a TIMEOUT error
will be returned.
@param messageType the type of message that you are waiting for. Once this message type is received
the method will return with a status of SUCCESS.
@param timeoutSecs how many seconds to wait for the desired message type 
@note If you need to receive messages in the meantime then return control to application instead.
This is enabled by compile options in the various files. For example, in afSendData, define 
AF_DATA_CONFIRM_HANDLED_BY_APPLICATION.
*/
moduleResult_t waitForMessage(uint16_t messageType, uint8_t timeoutSecs)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( ((messageType == 0) || (timeoutSecs == 0)), METHOD_WAIT_FOR_MESSAGE);     
    
    /** How often to check that the module has a message */
#define WFM_POLL_INTERVAL_MS   100
    
     uint16_t intervals = timeoutSecs * 1000 / WFM_POLL_INTERVAL_MS; //how many times to check   
    //for (int i=0; i<intervals; i++)
     while (intervals--)
    {
        if (moduleHasMessageWaiting())                           // If there's a message waiting for us
        {
          getMessage();
          
            if (zmBuf[SRSP_LENGTH_FIELD] > 0)
            {
            	uint16_t rcvMsgType = CONVERT_TO_INT(zmBuf[2], zmBuf[1]);
                if (rcvMsgType == messageType)
                {
#ifdef ZM_INTERFACE_VERBOSE
                    printf("Received expected message %04X\r\n", messageType);
#endif
                    return MODULE_SUCCESS;
                } else {                                            //not what we wanted; ignore
#ifdef ZM_INTERFACE_VERBOSE
                    printf("Received message %04X\r\n", rcvMsgType);
#endif 
                }
            }
        }
        delayMs(WFM_POLL_INTERVAL_MS);
    }
                                                 // We've completed without receiving the state that we want
    RETURN_RESULT(TIMEOUT, METHOD_WAIT_FOR_MESSAGE);    
}

#define METHOD_GET_CONFIGURATION_PARAMETER                    0x1000
/** 
Retrieves the given configuration parameter from the Module. 
@param configId the configuration parameter to retrieve. See section "Z-Stack Common Definitions" in 
module.h for a list of valid config parameters. Not checked for validity.
@post zmBuf contains the specified parameter, starting at ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD
@note Does not verify configId is a valid configId
@note Does not verify length returned in the SRSP matches what the length should be for that configId.
*/
moduleResult_t getConfigurationParameter(uint8_t configId)
{
#define LENGTH_OF_LARGEST_CONFIG_PARAMETER 17  //ZCD_NV_USERDESC is largest
#define ZB_READ_CONFIGURATION_PAYLOAD_LEN 1
    zmBuf[0] = ZB_READ_CONFIGURATION_PAYLOAD_LEN;
    zmBuf[1] = MSB(ZB_READ_CONFIGURATION);
    zmBuf[2] = LSB(ZB_READ_CONFIGURATION);  
    
    zmBuf[3] = configId;
    RETURN_RESULT_IF_FAIL(sendMessage(), METHOD_GET_CONFIGURATION_PARAMETER);    

#define ZB_READ_CONFIGURATION_STATUS_FIELD            SRSP_PAYLOAD_START
    RETURN_RESULT(zmBuf[ZB_READ_CONFIGURATION_STATUS_FIELD], METHOD_GET_CONFIGURATION_PARAMETER);  

    /* If you would like to do more extensive error checking then replace the above line of code with:
    
#define CONFIG_ID_FIELD         SRSP_PAYLOAD_START + 1
#define LENGTH_FIELD            SRSP_PAYLOAD_START + 2
    //when using SPI, zmBuf[3] holds status, [4] holds configId, [5] holds length
    if ((zmBuf[STATUS_FIELD] != SRSP_STATUS_SUCCESS) ||                  //if status was success...
    (zmBuf[CONFIG_ID_FIELD] != configId) ||                          //and the requested configId was what we requested..
    (zmBuf[LENGTH_FIELD] > LENGTH_OF_LARGEST_CONFIG_PARAMETER) ||    //if the length is valid
    (zmBuf[LENGTH_FIELD] == 0))
    {
    *Return Error here* 

} else {
    RETURN_RESULT(zmBuf[ZB_READ_CONFIGURATION_STATUS_FIELD], METHOD_GET_CONFIGURATION_PARAMETER); 
}
    */ 
}

// Note: no method ID here since each call will report its error.
/** 
Utility method to display stored network configuration parameters. These are the configuration
parameters stored in NV memory and are used to initialize the Module.
@note These are just what is used on startup - this does not mean that the module is currently using
these settings. Use displayDeviceInformation() to see current values used.
*/
moduleResult_t displayNetworkConfigurationParameters()
{
    moduleResult_t result = MODULE_SUCCESS;
    printf("Module Configuration Parameters\n\r");
    
    result = getConfigurationParameter(ZCD_NV_PANID);
    if (result != MODULE_SUCCESS) return result;
    printf("    ZCD_NV_PANID                %04X\r\n", 
           (CONVERT_TO_INT(zmBuf[ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD], 
                           zmBuf[ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD+1])));
    
    result = getConfigurationParameter(ZCD_NV_CHANLIST);
    if (result != MODULE_SUCCESS) return result;
    printf("    ZCD_NV_CHANLIST             %02X %02X %02X %02X\r\n", 
           zmBuf[ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD], 
           zmBuf[ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD + 1], 
           zmBuf[ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD + 2], 
           zmBuf[ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD + 3]);
    
    result = getConfigurationParameter(ZCD_NV_SECURITY_MODE);
    if (result != MODULE_SUCCESS) return result;
    printf("    ZCD_NV_SECURITY_MODE        %02X\r\n", zmBuf[ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD]);
    
    result = getConfigurationParameter(ZCD_NV_PRECFGKEYS_ENABLE);
    if (result != MODULE_SUCCESS) return result;
    printf("    ZCD_NV_PRECFGKEYS_ENABLE    %02X\r\n", zmBuf[ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD]);    
    
    result = getConfigurationParameter(ZCD_NV_PRECFGKEY);
    if (result != MODULE_SUCCESS) return result;
    printf("    ZCD_NV_PRECFGKEY            ");    
    printHexBytes(zmBuf+ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD, ZCD_NV_PRECFGKEY_LEN);
    
    return MODULE_SUCCESS;
}

/** 
Retrieves the name of the device state in a human-readable format.
@param state The integer value for the ZDO state
@return the name of the ZDO state, or "Unknown" if not known.
*/
char* getDeviceStateName(uint8_t state)
{
    switch (state)
    {
    case DEV_HOLD:              return("DEV_HOLD");
    case DEV_INIT:              return("DEV_INIT");
    case DEV_NWK_DISC:          return("DEV_NWK_DISC");    
    case DEV_NWK_JOINING:       return("DEV_NWK_JOINING");
    case DEV_NWK_REJOIN:        return("DEV_NWK_REJOIN");
    case DEV_END_DEVICE_UNAUTH: return("DEV_END_DEVICE_UNAUTH");    
    case DEV_END_DEVICE:        return("DEV_END_DEVICE");
    case DEV_ROUTER:            return("DEV_ROUTER");    
    case DEV_COORD_STARTING:    return("DEV_COORD_STARTING");
    case DEV_ZB_COORD:          return("DEV_ZB_COORD");
    case DEV_NWK_ORPHAN:        return("DEV_NWK_ORPHAN");       
    default:                    return("Unknown");
    }
}

//Note: no unique error code for this method since it wraps other methods
/** 
Displays the radio's device Information Properties. Device Information Properties include: 
- <i>Device State</i> indicates whether the radio is on a network or not, and what type of device it is. 
This is a handy thing to check if things aren't operating correctly. 
If the device is starting as a coordinator, you'll see states of 01, 08, 08, then 09 once it has fully started.
- <i>MAC Address</i> (aka IEEE Address) is a globally unique serial number for this IC.
- <i>Device Short Address</i> is a network address assigned by the coordinator, similar to an IP Address in DHCP. 
The Coordinator always has a Short Address of 0.
- <i>Parent MAC Address</i> is the IEEE Address of this device's "parent", i.e. which device was used to join the network. 
For a router, once joined this parent MAC address is irrelevant. This DIP will NOT be updated if the network reforms.
For an end-device then this parent MAC address will always specify which router the end-device is joined to.
- <i>Channel</i> is which frequency channel the device is operating on.
- <i>PAN ID</i> (Personal Area Network Identifier) of the network is a unique number shared for all devices on the same network.
- <i>Extended PAN ID</i> of the network is the coordinator's MAC Address.

If device is not connected to a network then the Short Address fields will be 0xFEFF, 
the Parent MAC Address and channel will be 0, and the Extended PAN ID will be this device's MAC Address.
*/
moduleResult_t displayDeviceInformation()
{
    int i;
    printf("Device Information Properties (MSB first\n\r)");
    moduleResult_t result = MODULE_SUCCESS;
    
    result = zbGetDeviceInfo(DIP_STATE);
    if (result != MODULE_SUCCESS) return result;
    printf("    Device State:               %s (%u)\r\n", getDeviceStateName(zmBuf[SRSP_DIP_VALUE_FIELD]), (zmBuf[SRSP_DIP_VALUE_FIELD])); 
    
    result = zbGetDeviceInfo(DIP_MAC_ADDRESS);
    printf("    MAC Address:                ");
    if (result != MODULE_SUCCESS) return result;
    for (i = SRSP_DIP_VALUE_FIELD+7; i>=SRSP_DIP_VALUE_FIELD; i--)
        printf("%02X ", zmBuf[i]);
    printf("\n\r");
    
    result = zbGetDeviceInfo(DIP_SHORT_ADDRESS);
    if (result != MODULE_SUCCESS) return result;
    printf("    Short Address:              %04X\r\n", CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD] , zmBuf[SRSP_DIP_VALUE_FIELD+1]));
    
    result = zbGetDeviceInfo(DIP_PARENT_SHORT_ADDRESS);
    if (result != MODULE_SUCCESS) return result;
    printf("    Parent Short Address:       %04X\r\n", CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD] , zmBuf[SRSP_DIP_VALUE_FIELD+1]));
    
    result = zbGetDeviceInfo(DIP_PARENT_MAC_ADDRESS);
    if (result != MODULE_SUCCESS) return result;
    printf("    Parent MAC Address:         ");
    for (i = SRSP_DIP_VALUE_FIELD+7; i>=SRSP_DIP_VALUE_FIELD; i--)
        printf("%02X ", zmBuf[i]);
    printf("\n\r");
    
    result = zbGetDeviceInfo(DIP_CHANNEL);
    if (result != MODULE_SUCCESS) return result;
    printf("    Device Channel:             %u\r\n", zmBuf[SRSP_DIP_VALUE_FIELD]);
    
    result = zbGetDeviceInfo(DIP_PANID);
    if (result != MODULE_SUCCESS) return result;
    printf("    PAN ID:                     %04X\r\n", CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD], zmBuf[SRSP_DIP_VALUE_FIELD+1]));
    
    result = zbGetDeviceInfo(DIP_EXTENDED_PANID);
    if (result != MODULE_SUCCESS) return result;
    printf("    Extended PAN ID:            ");
    for (i = SRSP_DIP_VALUE_FIELD+7; i>=SRSP_DIP_VALUE_FIELD; i--)
        printf("%02X ", zmBuf[i]);
    printf("\n\r");
    
    return MODULE_SUCCESS;
}

/** 
A minimalized version of displayDeviceInformation() for use on memory-constrained devices.
@see displayDeviceInformation() for description of fields.
*/
moduleResult_t displayBasicDeviceInformation()
{
    moduleResult_t result = MODULE_SUCCESS;    
    int i;
    result = zbGetDeviceInfo(DIP_MAC_ADDRESS);
    printf("MAC Addr:0x");
    if (result != MODULE_SUCCESS) return result;
    for (i = SRSP_DIP_VALUE_FIELD+7; i>=SRSP_DIP_VALUE_FIELD; i--)
        printf("%02X", zmBuf[i]);
    printf("; ");
    
    result = zbGetDeviceInfo(DIP_SHORT_ADDRESS);
    if (result != MODULE_SUCCESS) return result;
    printf("Short Addr:0x%04X; ", CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD] , zmBuf[SRSP_DIP_VALUE_FIELD+1]));
    
    result = zbGetDeviceInfo(DIP_CHANNEL);
    if (result != MODULE_SUCCESS) return result;
    printf("Channel:%u; ", zmBuf[SRSP_DIP_VALUE_FIELD]);
    
    result = zbGetDeviceInfo(DIP_PANID);
    if (result != MODULE_SUCCESS) return result;
    printf("PAN ID:0x%04X\r\n", CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD], zmBuf[SRSP_DIP_VALUE_FIELD+1]));

    return MODULE_SUCCESS;
}

/** 
Utility method to display the entire contents of the Module Buffer, zmBuf. Not normally used in 
driver or examples but useful for debugging Module communications.
*/
void displayZmBuf()
{
    printf("zb: ");
    printHexBytes(zmBuf, zmBuf[0]+3);
	printf("\n\r");
}

//
//  Security
//


/** 
Utility method for displaying the name of the security mode
@param securityMode the securityMode we want to know the name of
@return the name of the securityMode, or "Unknown" if it is not known.
*/
char* getSecurityModeName(uint8_t securityMode)
{
    switch (securityMode)
    {
    case SECURITY_MODE_OFF:                 return("OFF");
    case SECURITY_MODE_PRECONFIGURED_KEYS:  return("USE PRECONFIGURED KEY");
    case SECURITY_MODE_COORD_DIST_KEYS:     return("COORDINATOR DISTRIBUTE KEY");   
    default:                                return("Unknown");
    }
}

#define METHOD_SET_SECURITY_MODE                    0x1200
/** 
Configures the Module for the specified security mode. If securityMode is SECURITY_MODE_OFF then 
only ZCD_NV_SECURITY_MODE is written. Otherwise ZCD_NV_SECURITY_MODE and ZCD_NV_PRECFGKEYS_ENABLE 
are written.
@note if NOT using pre-configured keys then the coordinator will distribute its key to all devices.
@param securityMode must be SECURITY_MODE_OFF, or SECURITY_MODE_PRECONFIGURED_KEYS, or 
SECURITY_MODE_COORD_DIST_KEYS. If none of these then an INVALID_PARAMETER error will be returned.
*/
moduleResult_t setSecurityMode(uint8_t securityMode)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( (securityMode > SECURITY_MODE_COORD_DIST_KEYS), METHOD_SET_SECURITY_MODE);     
    
#ifdef ZM_INTERFACE_VERBOSE     
    printf("Setting Security = %s\r\n", getSecurityModeName(securityMode));      
#endif
    uint8_t data[1];
    data[0] = (securityMode > 0);               // Configure security on/off:

    RETURN_RESULT_IF_FAIL(zbWriteConfiguration(ZCD_NV_SECURITY_MODE, ZCD_NV_SECURITY_MODE_LEN, data), METHOD_SET_SECURITY_MODE);      
    
    if (securityMode != SECURITY_MODE_OFF)      // If turning security off, don't need to set pre-configured keys on/off
    {
        data[0] = (securityMode == SECURITY_MODE_PRECONFIGURED_KEYS);        //Now, configure pre-configured keys on/off:
        RETURN_RESULT(zbWriteConfiguration(ZCD_NV_PRECFGKEYS_ENABLE, ZCD_NV_PRECFGKEYS_ENABLE_LEN, data), METHOD_SET_SECURITY_MODE);         
    } else {
        return MODULE_SUCCESS;        
    }
}


#define METHOD_SET_SECURITY_KEY                    0x1300
/** 
Loads the 16 byte security key pointed to by key into the Module. Does not change security mode, you
need to use setSecurityMode() too.
@note if NOT using pre-configured keys then the coordinator will distribute its key to all devices.
@param key preConfiguredKey a 16B key to use. Must not be null.
@pre setSecurityMode() called
@pre key points to the 16 byte security key you want to use
*/
moduleResult_t setSecurityKey(uint8_t* key)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( (key == NULL), METHOD_SET_SECURITY_KEY);
    
#ifdef MODULE_INTERFACE_VERBOSE     
    printf("Setting Security Key: ");
    printHexBytes(key, ZCD_NV_PRECFGKEY_LEN);
#endif
    RETURN_RESULT(zbWriteConfiguration(ZCD_NV_PRECFGKEY, ZCD_NV_PRECFGKEY_LEN, key), METHOD_SET_SECURITY_KEY);
}

#define METHOD_SET_POLL_RATE                    0x1400
/** 
Sets the ZCD_NV_POLL_RATE, which is used by end devices and controls how often an end device
polls its associated router for new data. The default is 2000 (2 seconds). Higher poll rates will 
increase latency but reduce average power consumption. If poll rate is set too high for the amount 
of data coming to the end device then packets may be dropped. If this parameter is set to zero, the 
device will not automatically wake up to poll for data. Instead, an external trigger or an internal 
event (for example, via a software timer event) can be used to wake up the device.
@param pollRate how often (in mSec) to poll; must be between 0 to 65000, inclusive.
@note for measuring sleep current consumption of an End Device, set pollRate to zero to turn off 
polling. This will stop the module from polling and make measurements easier.
@note This setting is only valid for End Devices
*/
moduleResult_t setPollRate(uint16_t pollRate)
{ 
    RETURN_INVALID_PARAMETER_IF_TRUE((pollRate > 65000), METHOD_SET_POLL_RATE);      

#ifdef MODULE_INTERFACE_VERBOSE     
    printf("Setting ZCD_NV_POLL_RATE to %u\r\n", pollRate);
#endif
    uint8_t data[2];
    data[0] = LSB(pollRate);
    data[1] = MSB(pollRate);
    RETURN_RESULT((zbWriteConfiguration(ZCD_NV_POLL_RATE, ZCD_NV_POLL_RATE_LEN, data)), METHOD_SET_POLL_RATE);
}    

#define METHOD_SYS_SET_TIME 0x1500

moduleResult_t sysSetTime(uint32_t clock){
	#define SYS_SET_TIME_PAYLOAD_LEN 0x0B
    zmBuf[0] = SYS_SET_TIME_PAYLOAD_LEN;
    zmBuf[1] = MSB(SYS_SET_TIME);
    zmBuf[2] = LSB(SYS_SET_TIME);
	/* UTCTime – 4 bytes – Number of seconds since 00:00:00 on January 1, 2000 */
	zmBuf[3] = LSB(clock);
    zmBuf[4] = (clock & 0xFF00) >> 8;
    zmBuf[5] = (clock & 0xFF0000) >> 16;
    zmBuf[6] = clock >> 24;
	
	zmBuf[7] = 0;	//Hour
	zmBuf[8] = 0;	//Minute
	zmBuf[9] = 0;	//Second
	zmBuf[10] = 0;	//Month
	zmBuf[11] = 0;	//Day
	zmBuf[12] = 0;	//Year - Century
	zmBuf[13] = 0;	//Year - Year
	
    RETURN_RESULT(sendMessage(), METHOD_SYS_SET_TIME); 
}

#define METHOD_SYS_GET_TIME 0x1600
moduleResult_t sysGetTime(){
	#define SYS_GET_TIME_PAYLOAD_LEN 0
    zmBuf[0] = SYS_GET_TIME_PAYLOAD_LEN;
    zmBuf[1] = MSB(SYS_GET_TIME);
    zmBuf[2] = LSB(SYS_GET_TIME);
	
    RETURN_RESULT(sendMessage(), METHOD_SYS_GET_TIME); 
}

#define METHOD_SYS_GET_ADC              0x1700

moduleResult_t sysADC(uint8_t channel, uint8_t resolution)
{
    RETURN_INVALID_PARAMETER_IF_TRUE( (channel > 0x0F) || (resolution > 0x03), METHOD_SYS_GET_ADC );
    
#define SYS_ADC_PAYLOAD_LEN 2
    zmBuf[0] = SYS_ADC_PAYLOAD_LEN;
    zmBuf[1] = MSB(SYS_ADC_READ);
    zmBuf[2] = LSB(SYS_ADC_READ);
 
    zmBuf[3] = channel;
    zmBuf[4] = resolution;
    RETURN_RESULT(sendMessage(), METHOD_SYS_GET_ADC);        
}