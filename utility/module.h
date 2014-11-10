/**
*  @file module.h
*
*  @brief  public methods for module.c
*
* $Rev: 1796 $
* $Author: dsmith $
* $Date: 2013-04-22 03:00:33 -0700 (Mon, 22 Apr 2013) $
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

#ifndef MODULE_H
#define MODULE_H

#include "module_commands.h"
#include "module_errors.h"
#include <stdint.h>



//
//  Library Methods
//

// RESET and SYS_RESET_IND
moduleResult_t moduleReset();
char* getResetReason(uint8_t reason);
void displaySysResetInd();
void moduleInit();
#define SYS_RESET_IND_START_FIELD       (SRSP_PAYLOAD_START)
#define SYS_RESET_IND_PRODUCTID_FIELD   (SYS_RESET_IND_START_FIELD + 2)

// SYS_VERSION
moduleResult_t sysVersion();
void displaySysVersion();
#define SYS_VERSION_RESULT_START_FIELD      (SRSP_PAYLOAD_START)
#define SYS_VERSION_RESULT_PRODUCTID_FIELD   (SRSP_PAYLOAD_START + 1)
#define SYS_VERSION_RESULT_FW_MAJOR_FIELD   (SRSP_PAYLOAD_START + 2)
#define SYS_VERSION_RESULT_FW_MINOR_FIELD   (SRSP_PAYLOAD_START + 3)
#define SYS_VERSION_RESULT_FW_BUILD_FIELD   (SRSP_PAYLOAD_START + 4)

// SYS_RANDOM
moduleResult_t sysRandom();
#define SYS_RANDOM_RESULT_LSB_FIELD (SRSP_PAYLOAD_START)
#define SYS_RANDOM_RESULT_MSB_FIELD (SRSP_PAYLOAD_START+1)
#define SYS_RANDOM_RESULT()           (CONVERT_TO_INT(zmBuf[SYS_RANDOM_RESULT_LSB_FIELD], zmBuf[SYS_RANDOM_RESULT_MSB_FIELD]))

// SYS_ADC
moduleResult_t sysADC(uint8_t channel, uint8_t resolution);
#define SYS_ADC_RESULT_LSB_FIELD (SRSP_PAYLOAD_START)
#define SYS_ADC_RESULT_MSB_FIELD (SRSP_PAYLOAD_START+1)
#define SYS_ADC_RESULT()           (CONVERT_TO_INT(zmBuf[SYS_ADC_RESULT_LSB_FIELD], zmBuf[SYS_ADC_RESULT_MSB_FIELD]))
#define ADC_CHANNEL_TEMPERATURE_READING	0x0E
#define ADC_CHANNEL_VOLTAGE_READING		0x0F
#define ADC_RESOLUTION_7_BIT			0x00
#define ADC_RESOLUTION_9_BIT			0x01
#define ADC_RESOLUTION_10_BIT			0x02
#define ADC_RESOLUTION_12_BIT			0x03


// SYS_TIME
moduleResult_t sysSetTime(uint32_t clock);
moduleResult_t sysGetTime();

#define SYS_TIME_UTC_FIELD (SRSP_PAYLOAD_START)
#define SYS_TIME_HOUR_FIELD	(SRSP_PAYLOAD_START+4)
#define SYS_TIME_MINUTE_FIELD	(SRSP_PAYLOAD_START+5)
#define SYS_TIME_SECOND_FIELD	(SRSP_PAYLOAD_START+6)
#define SYS_TIME_MONTH_FIELD	(SRSP_PAYLOAD_START+7)
#define SYS_TIME_DAY_FIELD	(SRSP_PAYLOAD_START+8)
#define SYS_TIME_YEAR_LSB_FIELD	(SRSP_PAYLOAD_START+9)
#define SYS_TIME_YEAR_MSB_FIELD	(SRSP_PAYLOAD_START+10)

#define SYS_TIME_LSB()	(CONVERT_TO_INT(zmBuf[SYS_TIME_UTC_FIELD], zmBuf[SYS_TIME_UTC_FIELD+1]))
#define SYS_TIME_MSB()	(CONVERT_TO_INT(zmBuf[SYS_TIME_UTC_FIELD+2], zmBuf[SYS_TIME_UTC_FIELD+3]))
#define SYS_TIME_HOUR()	zmBuf[SYS_TIME_HOUR_FIELD]
#define SYS_TIME_MINUTE()	zmBuf[SYS_TIME_MINUTE_FIELD]
#define SYS_TIME_SECOND()	zmBuf[SYS_TIME_SECOND_FIELD]
#define SYS_TIME_MONTH()	zmBuf[SYS_TIME_MONTH_FIELD]
#define SYS_TIME_DAY()	zmBuf[SYS_TIME_DAY_FIELD]
#define SYS_TIME_YEAR()	(CONVERT_TO_INT(zmBuf[SYS_TIME_YEAR_LSB_FIELD], zmBuf[SYS_TIME_YEAR_MSB_FIELD]))

//Miscellaneous
void displayZmBuf();
moduleResult_t waitForMessage(uint16_t messageType, uint8_t timeoutSecs);
    

//
//  Setting Channel
//
moduleResult_t setChannel(uint8_t channel);
moduleResult_t setChannelMask(uint32_t channelMask);
#define CHANNEL_MIN                             11
#ifdef INCLUDE_CHANNEL_26  // See setChannel() comments for explanation
#define CHANNEL_MAX                             26
#else
#define CHANNEL_MAX                             25
#endif
#define CHANNEL_MASK_11                     0x800
#define CHANNEL_MASK_12                    0x1000
#define CHANNEL_MASK_13                    0x2000
#define CHANNEL_MASK_14                    0x4000
#define CHANNEL_MASK_15                    0x8000
#define CHANNEL_MASK_16                   0x10000
#define CHANNEL_MASK_17                   0x20000
#define CHANNEL_MASK_18                   0x40000
#define CHANNEL_MASK_19                   0x80000
#define CHANNEL_MASK_20                  0x100000
#define CHANNEL_MASK_21                  0x200000
#define CHANNEL_MASK_22                  0x400000
#define CHANNEL_MASK_23                  0x800000
#define CHANNEL_MASK_24                 0x1000000
#define CHANNEL_MASK_25                 0x2000000
#define CHANNEL_MASK_26                 0x4000000

#ifdef INCLUDE_CHANNEL_26  // See setChannel() comments for explanation
#define ANY_CHANNEL_MASK                0x7FFF800  //Channel 11-26 bitmask
#else
#define ANY_CHANNEL_MASK                0x3FFF800  //Channel 11-25 bitmask
#endif


#define MIN_CHANNEL_MASK                CHANNEL_MASK_11
#define MAX_CHANNEL_MASK                ANY_CHANNEL_MASK
#define IS_VALID_CHANNEL(channel)       ((channel>=CHANNEL_MIN) && (channel<=CHANNEL_MAX))

//
//Setting PAN ID
//
moduleResult_t setPanId(uint16_t panId);
#define ANY_PAN                         0xFFFF
#define MAX_PANID                       0xFFF7
#define IS_VALID_PANID(id)              (id<=MAX_PANID)

//
//  Device Information Properties
//
moduleResult_t zbGetDeviceInfo(uint8_t dip);
moduleResult_t displayDeviceInformation();
moduleResult_t displayBasicDeviceInformation();
#define DIP_STATE                       0x00
#define DIP_MAC_ADDRESS                 0x01
#define DIP_SHORT_ADDRESS               0x02
#define DIP_PARENT_SHORT_ADDRESS        0x03
#define DIP_PARENT_MAC_ADDRESS          0x04
#define DIP_CHANNEL                     0x05
#define DIP_PANID                       0x06
#define DIP_EXTENDED_PANID              0x07
#define MAX_DEVICE_INFORMATION_PROPERTY 0x07
//Field Lengths
#define DIP_MAC_ADDRESS_LENGTH          8
#define DIP_SHORT_ADDRESS_LENGTH        2
#define DIP_PARENT_SHORT_ADDRESS_LENGTH 2
#define DIP_PARENT_MAC_ADDRESS_LENGTH   8
#define DIP_CHANNEL_LENGTH              1
#define DIP_PANID_LENGTH                2
#define DIP_EXTENDED_PANID_LENGTH       8
//Values for DIP_STATE:
#define DEV_HOLD                        0
#define DEV_INIT                        1
#define DEV_NWK_DISC                    2
#define DEV_NWK_JOINING                 3
#define DEV_NWK_REJOIN                  4
#define DEV_END_DEVICE_UNAUTH           5
#define DEV_END_DEVICE                  6
#define DEV_ROUTER                      7
#define DEV_COORD_STARTING              8
#define DEV_ZB_COORD                    9
#define DEV_NWK_ORPHAN                  10
#define MAX_DEVICE_STATE                10
#define SRSP_DIP_VALUE_FIELD (SRSP_HEADER_SIZE+1) //index in zmBuf[] of the start of the Device Information Property field. LSB is first.
#define IS_VALID_DEVICE_STATE(state)    (state <= MAX_DEVICE_STATE)
#define SRSP                            0x4000
#define MODULE_COMMAND()                (CONVERT_TO_INT(zmBuf[SRSP_CMD_LSB_FIELD], zmBuf[SRSP_CMD_MSB_FIELD]))
#define IS_SYS_RESET_IND()              (MODULE_COMMAND() == SYS_RESET_IND)
#define IS_SYS_VERSION_SRSP()           (MODULE_COMMAND() == (SYS_VERSION + SRSP))

char* getDeviceStateName(uint8_t state);

//
//  Z-Stack Common Definitions (ZCD) Items
//
moduleResult_t getConfigurationParameter(uint8_t configId);
#define ZB_READ_CONFIGURATION_START_OF_VALUE_FIELD    SRSP_PAYLOAD_START + 3
moduleResult_t displayNetworkConfigurationParameters();
//General
#define ZCD_NV_USERDESC                 0x81
#define ZCD_NV_USERDESC_LEN             17
#define ZCD_NV_CHANLIST                 0x84
#define ZCD_NV_CHANLIST_LEN             4
#define ZCD_NV_PANID                    0x83
#define ZCD_NV_PANID_LEN                2
#define ZCD_NV_STARTUP_OPTION           0x03
#define ZCD_NV_STARTUP_OPTION_LEN       1
#define ZCD_NV_LOGICAL_TYPE             0x87
#define ZCD_NV_LOGICAL_TYPE_LEN         1
#define ZCD_NV_ZDO_DIRECT_CB            0x8F
#define ZCD_NV_ZDO_DIRECT_CB_LEN        1
#define ZCD_NV_POLL_RATE                0x24
#define ZCD_NV_POLL_RATE_LEN            2
#define ZCD_NV_QUEUED_POLL_RATE         0x25
#define ZCD_NV_QUEUED_POLL_RATE_LEN     2
#define ZCD_NV_RESPONSE_POLL_RATE       0x26
#define ZCD_NV_RESPONSE_POLL_RATE_LEN   2
//For Security:
#define ZCD_NV_PRECFGKEY                0x62
#define ZCD_NV_PRECFGKEY_LEN            16
#define ZCD_NV_PRECFGKEYS_ENABLE        0x63
#define ZCD_NV_PRECFGKEYS_ENABLE_LEN    1
#define ZCD_NV_SECURITY_MODE            0x64
#define ZCD_NV_SECURITY_MODE_LEN        1

//
//  Security
//
char* getSecurityModeName(uint8_t securityMode);
moduleResult_t setSecurityMode(uint8_t securityMode);
moduleResult_t setSecurityKey(uint8_t* key);
// Security Modes:
#define SECURITY_MODE_OFF                0
#define SECURITY_MODE_PRECONFIGURED_KEYS 1
#define SECURITY_MODE_COORD_DIST_KEYS    2

//
//  Non-volatile memory item storage
//
moduleResult_t sysNvRead(uint8_t nvItem);
#define SYS_NV_READ_STATUS_FIELD        (SRSP_PAYLOAD_START)
#define SYS_NV_READ_RESULT_START_FIELD  (SRSP_PAYLOAD_START + 2)

moduleResult_t sysNvWrite(uint8_t nvItem, uint8_t* data);
#define SYS_NV_WRITE_STATUS_FIELD        (SRSP_PAYLOAD_START)
uint8_t getNvItemSize(uint8_t nvItem);
#define MIN_NV_ITEM                     1
#define MAX_NV_ITEM                     9
#define MAX_NV_ITEM_READ                MAX_NV_ITEM
#define MAX_NV_ITEM_USER                6
#define NV_ITEM_RESERVED                7 // Do not use

//
//      GPIO Pin read/write
//
moduleResult_t sysGpio(uint8_t operation, uint8_t value);
#define SYS_GPIO_READ_RESULT_FIELD   (SRSP_PAYLOAD_START)

//Operations:
#define GPIO_SET_DIRECTION      0x00
#define GPIO_SET_INPUT_MODE     0x01
#define GPIO_SET                0x02
#define GPIO_CLEAR              0x03
#define GPIO_TOGGLE             0x04
#define GPIO_READ               0x05
#define GPIO_OPERATION_MAX      0x05
//GPIO pin definitions
#define ALL_GPIO_PINS           0x0F  //GPIO 0-3
#define GPIO_0                  0x01
#define GPIO_1                  0x02
#define GPIO_2                  0x04
#define GPIO_3                  0x08
//options for GPIO_SET_INPUT_MODE
#define GPIO_INPUT_MODE_ALL_PULL_DOWNS  0xF0
#define GPIO_INPUT_MODE_ALL_PULL_UPS    0x00
#define GPIO_INPUT_MODE_ALL_TRI_STATE   0x0F
#define GPIO_DIRECTION_ALL_INPUTS       0x00



//
//  RF Test
//
moduleResult_t setRfTestMode(uint8_t mode, uint8_t channel, uint8_t txPower, uint8_t txTone);
char* getRfTestModeName(uint8_t mode);
//RF TEST MODES
#define RF_TEST_NONE                    0
#define RF_TEST_RECEIVE                 1
#define RF_TEST_UNMODULATED             2
#define RF_TEST_MODULATED               3
#define RF_TEST_MODE_MAXIMUM            3
//RF Transmit Power values, from Table 1 in cc2530 datasheet, p 21
#define RF_OUTPUT_POWER_PLUS_4_5_DBM    0xF5  //+4.5dBm
#define RF_OUTPUT_POWER_PLUS_2_5_DBM    0xE5  //+2.5dBm
#define RF_OUTPUT_POWER_PLUS_1_0_DBM    0xD5  //+1.0dBm
#define RF_OUTPUT_POWER_MINUS_0_5_DBM   0xC5  //-0.5dBm
#define RF_OUTPUT_POWER_MINUS_1_5_DBM   0xB5  //-1.5dBm
#define RF_OUTPUT_POWER_MINUS_3_0_DBM   0xA5  //-3.0dBm
#define RF_OUTPUT_POWER_MINUS_10_0_DBM  0x65  //-10.0dBm
#define RF_OUTPUT_POWER_MINUS_12_0_DBM  0x55  //-10.0dBm
#define RF_OUTPUT_POWER_MINUS_14_0_DBM  0x45  //-10.0dBm
#define RF_OUTPUT_POWER_MINUS_16_0_DBM  0x35  //-10.0dBm
#define RF_OUTPUT_POWER_MINUS_18_0_DBM  0x25  //-10.0dBm
#define RF_OUTPUT_POWER_MINUS_20_0_DBM  0x15  //-20.0dBm
#define RF_OUTPUT_POWER_MINUS_22_0_DBM  0x05  //-10.0dBm

//
//  Set Device Type
//
moduleResult_t setZigbeeDeviceType(uint8_t deviceType);
#define COORDINATOR                     0x00
#define ROUTER                          0x01
#define END_DEVICE                      0x02
#define IS_VALID_ZIGBEE_DEVICE_TYPE(type)   ((type == COORDINATOR) || (type == ROUTER) || (type == END_DEVICE))

//
//  Set Startup Options
//
moduleResult_t setStartupOptions(uint8_t option);
#define STARTOPT_CLEAR_CONFIG           0x01
#define STARTOPT_CLEAR_STATE            0x02
#define STARTOPT_AUTO                   0x04

//
//  Set Callbacks
//
moduleResult_t setCallbacks(uint8_t cb);
#define CALLBACKS_DISABLED              0
#define CALLBACKS_ENABLED               1

//
//  Set Tx Power
//
#define SYS_SET_TX_POWER_RESULT_FIELD        (SRSP_PAYLOAD_START)
moduleResult_t sysSetTxPower(uint8_t txPowerDb, uint8_t* actualPowerDb);

//
//  Advanced Commands
//
moduleResult_t setPollRate(uint16_t pollRate);

//
//  Misc. defines used in ZB_SEND_DATA_REQUEST & AF_DATA_REQUEST
//
#define DEFAULT_RADIUS                  0x0F    //Maximum number of hops to get to destination
#define MAXIMUM_PAYLOAD_LENGTH          81      //Updated in 2.4.0: 99B w/o security, 81B w/ NWK security, 66B w/ APS security
#define ALL_DEVICES                     0xFFFF
#define ALL_ROUTERS_AND_COORDINATORS    0xFFFC




#endif
