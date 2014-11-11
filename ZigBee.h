#ifndef ZigBee_h
#define ZigBee_h

#include "Energia.h"
#include <stdint.h>
#include "utility/_SETTINGS_.h"
#include "utility/module.h"
#include "utility/module_utilities.h"
#include "utility/application_configuration.h"
#include "utility/af.h"

#include "Print.h"
#include "Stream.h"
//#include "MACAddress.h"


#ifdef __MSP430G2553
#define MAX_MESSAGE_SIZE		32
#define MAX_APPLICATION_SIZE	1
#else
#define MAX_APPLICATION_SIZE	4
#define MAX_MESSAGE_SIZE		128
#endif


#define SUCCESS					0
#define FAIL					1

#define BROADCAST_ADDRESS 		0xFFFF

#define SELF					(uint8_t) 0x00
#define PARENT 					(uint8_t) 0x01
#define FROM					(uint8_t) 0x02
#define TO						(uint8_t) 0x03
#define READ					(uint8_t) 0x04
#define INCOMING				FROM
#define OUTGOING				TO


#define CURRENT					0x00
#define RECEIVED_TIMESTAMP		0x01

// RECEIVED MESSAGE PARAMETERS  0x01XD x = method d = size
#define LQI						0x0101u
#define FROM_ENDPOINT			0x0111u
#define TO_ENDPOINT				0x0121u
#define WAS_BROADCAST			0x0131u
#define CAPABILITIES			0x0141u
#define TRANSACTION				0x0151u
#define FROM_ADDRESS			0x0162u
#define TO_ADDRESS				0x0172u
#define CLUSTER_ID				0x0182u
#define GROUP_ID				0x0192u
#define TIMESTAMP				0x01A4u
#define FROM_MACADDRESS			0x01B8u
#define TYPE					0x01C2u
#define LENGTH					0x01D2u
// MESSAGE TYPES
#define STATE_CHANGE			ZDO_STATE_CHANGE_IND        // 0x45C0
#define DEVICE_ANNOUNCE			ZDO_END_DEVICE_ANNCE_IND	// 0x45C1
#define DEVICE_LEAVE			ZDO_LEAVE_IND        		// 0x45C9 
#define INCOMING_DATA			AF_INCOMING_MSG				// 0x4481
#define DATA_CONFIRM			AF_DATA_CONFIRM				// 0x4480

// MODULE PARAMETERS			0x00XD
#define STATE					0x0001u						// 0x00
#define MAC_ADDRESS				0x0018u						// 0x01
#define ADDRESS               	0x0022u						// 0x02
#define PARENT_ADDRESS        	0x0032u						// 0x03
#define PARENT_MAC_ADDRESS      0x0048u   					// 0x04
#define CHANNEL					0x0052u						// 0x05
#define PANID					0x0061u						// 0x06
#define EXTENDED_PANID			0x0072u						// 0x07
#define RANDOM					0x00A2u
#define DATETIME				0x00B4u
#define VOLTAGE					0x00C2u
#define TEMPERATURE				0x00D2u




// DATA FORMATS - determines how data is printed and typecast
#define CHAR                    0xFA00u
#define UINT8					0xFB01u
#define UINT16					0xFB02u
#define UINT32					0xFB04u
#define UINT64					0xFB08u
#define INT8					0xFB11u
#define INT16					0xFB12u
#define INT32					0xFB14u
#define INT64					0xFB18u
#define MAC						0xFC08u


union mac_t {
  byte num[8];
  uint64_t num64;
};

class ZigBeeClass: public Stream
{

private:
	uint8_t index;
	uint8_t _status;
	uint8_t buffer[MAX_MESSAGE_SIZE];
	
	
	uint16_t receivedLength;
	uint16_t receivedType;
	uint8_t receivedCapabilities;
	uint16_t receivedClusterId;
	uint8_t  receivedLqi;	
	uint8_t receivedWasBroadcast;
	uint16_t receivedGroup;
	uint16_t receivedToAddress;		
	uint16_t receivedFromAddress;	
	uint8_t receivedToEndpoint;
	uint8_t receivedFromEndpoint;
	uint8_t receivedTransaction;
	uint32_t receivedTimestamp;
	uint64_t receivedMac;
	
	//MACAddress receivedMac;
	
#ifndef __MSP430G2553
	static void (*user_onReceive)(void);
	uint8_t _applicationCount;
#endif
	int start();
	//void reverseMac(uint8_t* buf);
public:

	ZigBeeClass();

#ifndef __MSP430G2553
	struct applicationConfiguration application[MAX_APPLICATION_SIZE];
	void onReceive( void (*)(void) );	// ONLY USE IF YOU KNOW WHAT YOU ARE DOING, useful for time sensitive methods
#else
	struct applicationConfiguration application;
#endif
	struct moduleConfiguration config;


/******************* HAL Configuration ***************************/

	// THESE METHODS CAN ALSO BE READ/WRITE BY USING ZigBee.hal.<variable>

	void mrstPin(uint8_t pin);
	void mrdyPin(uint8_t pin);
	void srdyPin(uint8_t pin);
	void spiModule(uint8_t module);

/******************* MODULE Configuration ***************************/

	// THESE METHODS CAN ALSO BE CALLED/READ BY USING ZigBee.config.<variable>


	// OPERATING REGION: MODULE_REGION_NORTH_AMERICA, MODULE_REGION_EUROPE
	// DEFAULT: MODULE_REGION_NORTH_AMERICA
	void operatingRegion(uint16_t _region);
	
	// CHANNEL MASK: CHANNEL_MASK_11 to CHANNEL_MASK_26
	// DEFAULT: (CHANNEL_MASK_11 | CHANNEL_MASK_14 | CHANNEL_MASK_17 | CHANNEL_MASK_20 | CHANNEL_MASK_23)
	void channelMask(uint32_t _channelMask);  
	
	// PANID 0x0000 TO 0x3FFF, SET TO 0xFFFF FOR DON'T CARE
	// DEFAULT: 0xFFFF
	void panId(uint16_t _panId);
	
	// ENDDEVICEPOLLRATE: 0 - 65536, MS FOR MODULES TO WAKE UP AND POLL FOR MESSAGES
	// DEFAULT: 2000
	void endDevicePollRate(uint16_t _endDevicePollRate);
	
	// STARTUP OPTIONS: STARTOPT_CLEAR_CONFIG, STARTOPT_CLEAR_STATE, STARTOPT_AUTO
	// DEFAULT: (STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE)
	void startupOptions(uint8_t _startupOptions);
	
	// SECURITY MODE: SECURITY_MODE_OFF, SECURITY_MODE_PRECONFIGURED_KEYS, SECURITY_MODE_COORD_DIST_KEYS
	// DEFAULT: SECURITY_MODE_OFF
	void securityMode(uint8_t _securityMode);
	
	// SECURITY KEY: 16 byte (128 bit) security key
	// DEFAULT: NULL
	void securityKey(char* _securityKey);
	
	// MODULE METHODS
	
	// DEVICE TYPE PARAMETERS: COORDINATOR, ROUTER, END_DEVICE
	// DEFAULT: COORDINATOR
	void deviceType(uint8_t _deviceType);


/************************** Module Methods ***********************************************/
	
	// DEVICE TYPE PARAMETERS: COORDINATOR, ROUTER, END_DEVICE
	// DEFAULT: COORDINATOR
	int begin(uint8_t deviceType, uint8_t securityMode, char* securityKey);
	int begin(uint8_t deviceType);
	int begin();
	int reconnect();
	void stop(); // turns off ZigBee radio


/******************* AF Functions ***************************/

	int send();
	int send(uint16_t shortAddress);
	int send(uint16_t shortAddress, uint8_t toEndpoint, uint8_t fromEndpoint, uint16_t cluster);
	int bindcast(); //send message to binded address(s)
	int bindcast(uint8_t toEndpoint, uint8_t fromEndpoint, uint16_t cluster); //send message to binded address(s)
	int groupcast(uint16_t groupAddress);
	int broadcast();
	int broadcast(uint8_t toEndpoint, uint8_t fromEndpoint, uint16_t cluster);
	void ackMode(uint8_t ack); // ACK MODE: AF_MAC_ACK, AF_APS_ACK
	int receive();
	int receive(uint16_t messageType);
	
/******************* ZDO Functions ***************************/

	int permit(uint16_t destAddress, uint8_t permitseconds);
	uint16_t address(uint64_t addressname); 	// get the network address of a device given its network address. Use PARENT for Parent, leave blank for current device.
	uint64_t macAddress(uint16_t addressname);			// get the mac address of a device given its network address. Use PARENT for Parent, leave blank for current device.

	//MACAddress macAddress();
	int bind(uint16_t addressname);			// binds current device (with default endpoint) to addressname, so that current device can send messages with bindcast()
	int unbind(uint16_t addressname);			// binds addressname to current device, so that device at addressname can send messages with bindcast
	int bind(uint16_t addressname, uint64_t fromMac, uint64_t toMac);
	int unbind(uint16_t addressname, uint64_t fromMac, uint64_t toMac);
	int bind(uint16_t addressname, uint8_t srcEndpoint, uint64_t sourceMac, uint8_t destinationEndpoint, uint64_t destinationMac, uint16_t cluster);
	int unbind(uint16_t addressname, uint8_t srcEndpoint, uint64_t sourceMac, uint8_t destinationEndpoint, uint64_t destinationMac, uint16_t cluster);
	int bindGroup(uint16_t addressname, uint64_t sourceMac, uint16_t group);
	int unbindGroup(uint16_t addressname, uint64_t sourceMac, uint16_t group);
	
	
/******************* SYS Functions ***************************/

	uint32_t module(uint16_t parameter);

	uint16_t address(uint8_t addressType);
	uint16_t address();

	uint64_t macAddress(uint8_t addressType);
	uint64_t macAddress();
	//MACAddress macAddress();

	uint8_t endpoint(uint8_t addressType);
	uint8_t endpoint();
	
	uint16_t panId();
	uint8_t channel();
	uint8_t state();
	
	uint32_t time();
	int time(uint32_t clock);
	bool connected();

/******************* MESSAGE FUNCTIONS *************************/

	uint32_t received(uint16_t);
	uint8_t lqi();
	uint16_t cluster();
	uint16_t cluster(uint8_t);

	size_t write(uint8_t);
	size_t write(uint16_t);
	size_t write(uint32_t);
	size_t write(uint64_t);	
	size_t write(const uint8_t *buf, size_t size);
	size_t write(char *buf);
	
	int available();
	int read();
	uint64_t read(uint16_t numBytes);	// read up to 8 bytes as an integer
    int read(unsigned char* buffer, size_t size);
    int read(char* buffer, size_t size) { return read((unsigned char*)buffer, size); };
	void printTo(Print& p, uint16_t datatype);
	void printTo(Print& p, uint16_t datatype, int format);
	void printlnTo(Print& p, uint16_t datatype);
	void printlnTo(Print& p, uint16_t datatype, int format);
	void printTo(Print& p, uint16_t datatype, uint8_t count, int format, char separator);
	void printlnTo(Print& p, uint16_t datatype, uint8_t count, int format, char separator);
	
	int peek();
	uint64_t peek(uint16_t numbytes);
	void flush();

/***************************** APPLICATION FUNCTIONS ********************************/

    uint8_t appAdd();
	uint8_t appNum();
	uint8_t appDelete();

};

extern ZigBeeClass ZigBee;



#endif
