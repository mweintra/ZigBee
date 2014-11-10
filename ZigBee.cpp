

#include "ZigBee.h"
#include <stdint.h>
#include <inttypes.h>

#include "utility/HAL.h"
#include "utility/utilities.h"
#include "utility/module_errors.h"
#include "utility/zm_phy_spi.h"
#include "utility/module.h"
#include "utility/module_utilities.h"
#include "utility/application_configuration.h"
#include "utility/af.h"
#include "utility/zdo.h"
#include "utility/module_commands.h"

// Union to convert mac address into 64bit number. Useful for quick comparisons and moving data type


moduleResult_t result = MODULE_SUCCESS;

#ifndef __MSP430G2553
void (*ZigBeeClass::user_onReceive)(void);
#endif

ZigBeeClass::ZigBeeClass(){
	appCount=1;
#ifndef __MSP430G2553
	application[0]=DEFAULT_APPLICATION_CONFIGURATION;
#else
	application=DEFAULT_APPLICATION_CONFIGURATION;
#endif
	config=DEFAULT_MODULE_CONFIGURATION_COORDINATOR;
	afSetAckMode(AF_MAC_ACK);
	#if defined(__MSP430G2553)
		hal.mrstPin=P2_7;
		hal.mrdyPin=P2_0;
		hal.srdyPin=P2_2;
	#endif
	#if defined(__MSP430FR5969)
		hal.mrstPin=P3_0;
		hal.mrdyPin=P3_4;
		hal.srdyPin=P3_6;
	#endif
	#if defined(__MSP430F5529)
		hal.mrstPin=P2_2;
		hal.mrdyPin=P2_7;
		hal.srdyPin=P4_1;
	#endif
	#if defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM____) 
		hal.mrstPin=PE_0;
		hal.mrdyPin=PA_5;
		hal.srdyPin=PA_7;
		hal.spiModule=2;
	#endif
	#if defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__) 
		hal.mrstPin=PH_2;
		hal.mrdyPin=PC_7;
		hal.srdyPin=PB_3;
		hal.spiModule=2;	
	#endif
	#if defined(TARGET_IS_CC3101) 
		hal.mrstPin=18;
		hal.mrdyPin=8;
		hal.srdyPin=10;
	#endif	
}
#ifndef __MSP430G2553
void ZigBeeClass::onReceive( void (*function)(void) )
{
	user_onReceive = function;
	attachInterrupt(hal.srdyPin,user_onReceive,FALLING);
}
#endif
/******************** SET HARDWARE CONFIGURATIONS ****************************/

void ZigBeeClass::mrstPin(uint8_t pin){
	hal.mrstPin=pin;
}
void ZigBeeClass::mrdyPin(uint8_t pin){
	hal.mrdyPin=pin;
}
void ZigBeeClass::srdyPin(uint8_t pin){
	hal.srdyPin=pin;
}
void ZigBeeClass::spiModule(uint8_t module){
	hal.spiModule=module;
}


/********************************** MODULE METHODS ********************************/

int ZigBeeClass::begin(uint8_t deviceType) {
	config.deviceType=deviceType;
	return begin();
}

int ZigBeeClass::begin() {
	halInit();
    moduleInit();
	moduleReset();
	start();
}

int ZigBeeClass::reconnect() {
	uint16_t tempOptions = config.startupOptions;
	config.startupOptions=0;
	start();
	config.startupOptions=tempOptions;
}

int ZigBeeClass::start(){
	index=0;
#ifdef __MSP430G2553
	if ((result = startModule(&config, &application)) != MODULE_SUCCESS)
#else
	if ((result = startModule(&config, application)) != MODULE_SUCCESS)
#endif
	
    {
        printf("\n\rModule start unsuccessful. Error Code 0x%02X.", result);
    }else{
		printf("\n\rSuccess!\n\r"); 
	}
	return result;
}


int ZigBeeClass::bindcast(){
	result=afSendDataExtended(DEFAULT_ENDPOINT, DEFAULT_ENDPOINT, NULL, DESTINATION_ADDRESS_MODE_NONE, INFO_MESSAGE_CLUSTER, buffer, index);
	index=0;
	return result;
}

int ZigBeeClass::bindcast(uint8_t toEndpoint, uint8_t fromEndpoint, uint16_t cluster){
//	uint8_t destinationAddress[8];
	result=afSendDataExtended(toEndpoint, fromEndpoint, NULL, DESTINATION_ADDRESS_MODE_NONE, cluster, buffer, index);
	index=0;
	return result;
}

int ZigBeeClass::groupcast(uint16_t groupname){
	uint8_t groupAddress[2];
	groupAddress[0]=LSB(groupname);
	groupAddress[1]=MSB(groupname);
	result=afSendDataExtended(DEFAULT_ENDPOINT, DEFAULT_ENDPOINT, groupAddress, DESTINATION_ADDRESS_MODE_GROUP, INFO_MESSAGE_CLUSTER, buffer, index);
	index=0;
	return result;
}

int ZigBeeClass::broadcast(){
	result=afSendData(DEFAULT_ENDPOINT, DEFAULT_ENDPOINT, BROADCAST_ADDRESS, INFO_MESSAGE_CLUSTER, buffer, index);
	index=0;
	return result;
}

int ZigBeeClass::broadcast(uint8_t toEndpoint, uint8_t fromEndpoint, uint16_t cluster){
	result=afSendData(toEndpoint, fromEndpoint, BROADCAST_ADDRESS, cluster, buffer, index);
	index=0;
	return result;
}

int ZigBeeClass::send(uint16_t shortAddress){
	result=afSendData(DEFAULT_ENDPOINT, DEFAULT_ENDPOINT, shortAddress, INFO_MESSAGE_CLUSTER, buffer, index);
	index=0;
	return result;
}

int ZigBeeClass::send(uint16_t shortAddress, uint8_t toEndpoint, uint8_t fromEndpoint, uint16_t cluster){
	result=afSendData(toEndpoint, fromEndpoint, shortAddress, cluster, buffer, index);
	index=0;
	return result;
}

int permit(uint16_t destAddress, uint8_t permitseconds){
	return zdoManagementPermitJoinRequest(destAddress, permitseconds, 0);
}

int ZigBeeClass::receive(){
	receive(0);
}

int ZigBeeClass::receive(uint16_t messageType){
    delay(50);
	if(moduleHasMessageWaiting()){
		getMessage();
		if (zmBuf[SRSP_LENGTH_FIELD] > 0){
			receivedType=(CONVERT_TO_INT(zmBuf[SRSP_CMD_LSB_FIELD], zmBuf[SRSP_CMD_MSB_FIELD]));
			if (receivedType!=messageType && messageType!=0) return 0;
			if (IS_ZDO_END_DEVICE_ANNCE_IND()) {
				receivedFromAddress=GET_ZDO_END_DEVICE_ANNCE_IND_SRC_ADDRESS();
				receivedToAddress=GET_ZDO_END_DEVICE_ANNCE_IND_FROM_ADDRESS();
				mac_t macAddr;
				for (int i=0; i<8; i++) {
					macAddr.num[i]=zmBuf[ZDO_END_DEVICE_ANNCE_IND_MAC_START_FIELD+i];
				}
				receivedMac=macAddr.num64;
				receivedCapabilities=zmBuf[ZDO_END_DEVICE_ANNCE_IND_CAPABILITIES_FIELD];
			} else if (IS_AF_INCOMING_MESSAGE()){
				// Load the ZM parameters
				receivedLqi=zmBuf[AF_INCOMING_MESSAGE_LQI_FIELD];
				receivedLength=zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_LEN_FIELD];
				receivedFromAddress=AF_INCOMING_MESSAGE_SHORT_ADDRESS();
				receivedGroup=AF_INCOMING_MESSAGE_GROUP();
				receivedWasBroadcast=zmBuf[AF_INCOMING_MESSAGE_WAS_BROADCAST_FIELD];
				receivedClusterId=AF_INCOMING_MESSAGE_CLUSTER();
				receivedFromEndpoint=zmBuf[AF_INCOMING_MESSAGE_SOURCE_EP_FIELD];
				receivedToEndpoint=zmBuf[AF_INCOMING_MESSAGE_DESTINATION_EP_FIELD];
				receivedTransaction=zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD];
				receivedTimestamp=(uint32_t) zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD];	
				receivedTimestamp+=(uint32_t) zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD+1]*256;
				receivedTimestamp+=(uint32_t) zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD+2]*65536;
				receivedTimestamp+=(uint32_t) zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD+3]*16777216;
				// Load the Message
				for ( int i=0;i<receivedLength;i++){
					buffer[i]=zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+i];
				}
			} else if (IS_AF_INCOMING_MESSAGE_EXT()){
				// Load the ZM parameters
				receivedLqi=zmBuf[AF_INCOMING_MESSAGE_EXT_LQI_FIELD];
				receivedLength=AF_INCOMING_MESSAGE_EXT_LENGTH();
				receivedFromAddress=AF_INCOMING_MESSAGE_EXT_SHORT_ADDRESS();
				receivedGroup=AF_INCOMING_MESSAGE_EXT_GROUP();
				receivedWasBroadcast=zmBuf[AF_INCOMING_MESSAGE_EXT_WAS_BROADCAST_FIELD];
				receivedClusterId=AF_INCOMING_MESSAGE_EXT_CLUSTER();
				receivedFromEndpoint=zmBuf[AF_INCOMING_MESSAGE_EXT_SOURCE_EP_FIELD];
				receivedToEndpoint=zmBuf[AF_INCOMING_MESSAGE_EXT_DESTINATION_EP_FIELD];
				receivedTimestamp=(uint32_t) zmBuf[AF_INCOMING_MESSAGE_EXT_TIMESTAMP_START_FIELD];	
				receivedTimestamp+=(uint32_t) zmBuf[AF_INCOMING_MESSAGE_EXT_TIMESTAMP_START_FIELD+1]*256;
				receivedTimestamp+=(uint32_t) zmBuf[AF_INCOMING_MESSAGE_EXT_TIMESTAMP_START_FIELD+2]*65536;
				receivedTimestamp+=(uint32_t) zmBuf[AF_INCOMING_MESSAGE_EXT_TIMESTAMP_START_FIELD+3]*16777216;				
				// Load the Message
				for ( int i=0;i<receivedLength;i++){
					buffer[i]=zmBuf[AF_INCOMING_MESSAGE_EXT_PAYLOAD_START_FIELD+i];
				}
			} else if (IS_AF_DATA_CONFIRM()){
			} 
		}
	}else
		return 0;
	index=0;
	return receivedType;
}

void ZigBeeClass::stop(){
	RADIO_OFF();
}

uint32_t ZigBeeClass::module(uint16_t parameter){
	if (parameter<0x0080){
		result = zbGetDeviceInfo(0x000F & (parameter>>4) );
		if (result != MODULE_SUCCESS) return 0xFFFFFFFF;
		if (parameter==CHANNEL)
			return 0x000000FF & zmBuf[SRSP_DIP_VALUE_FIELD];
		else
			return 0x0000FFFF & (CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD], zmBuf[SRSP_DIP_VALUE_FIELD+1]));
	}else if(parameter==DATETIME){
		if ( sysGetTime()!=MODULE_SUCCESS ) return 0;
		return((uint32_t) SYS_TIME_MSB())*65536+(uint16_t)SYS_TIME_LSB();
	}else if (parameter==RANDOM){
		if (sysRandom()!=MODULE_SUCCESS) return 0;
		return 0x0000FFFF & SYS_RANDOM_RESULT();
	}else if(parameter==VOLTAGE){
		if (sysADC(ADC_CHANNEL_VOLTAGE_READING,ADC_RESOLUTION_12_BIT)!=MODULE_SUCCESS) return 0;
		return 0x0000FFFF & SYS_ADC_RESULT();
	} else if(parameter==TEMPERATURE){
		if (sysADC(ADC_CHANNEL_TEMPERATURE_READING,ADC_RESOLUTION_12_BIT)!=MODULE_SUCCESS) return 0;
		return 0x0000FFFF & SYS_ADC_RESULT();
	}
}

uint32_t ZigBeeClass::received(uint16_t parameter){
	if(parameter==receivedType){
		return 1;
	} else if(parameter==AF_INCOMING_MSG && receivedType==AF_INCOMING_MSG_EXT){
		return 2;
	} else if(parameter==FROM_ADDRESS || parameter==ADDRESS){
		return receivedFromAddress;
	} else if(parameter==TO_ADDRESS){
		return receivedToAddress;
	}else if(parameter==LQI){
		return receivedLqi;
	}else if (parameter==FROM_ENDPOINT){
		return receivedFromEndpoint;
	}else if (parameter==TO_ENDPOINT){
		return receivedToEndpoint;
	}else if (parameter==CLUSTER_ID){
		return receivedClusterId;
	}else if (parameter==GROUP_ID){
		return receivedGroup;
	}else if(parameter==WAS_BROADCAST){
		return receivedWasBroadcast;
	}else if(parameter==CAPABILITIES){
		return receivedCapabilities;
	}else if(parameter==TRANSACTION){
		return receivedTransaction;
	}else if(parameter==TIMESTAMP){
		return receivedTimestamp;
	}else if(parameter==LENGTH){
		return receivedLength;
	}else if(parameter==TYPE){
		return receivedType;
	}
	
	return 0;
}

bool ZigBeeClass::connected(){
	uint8_t tries;
	while(result=zbGetDeviceInfo(DIP_STATE) != MODULE_SUCCESS){
		tries++;
		delay(50);
		if (tries>5) return false;
	}
	uint8_t state=zmBuf[SRSP_DIP_VALUE_FIELD];
	return (state==getDeviceStateForDeviceType(config.deviceType));
}

void reversemac(uint8_t* buf){
	uint8_t temp;
	for(int i=0;i<4;i++){
		temp=buf[i];
		buf[i]=buf[7-i];
		buf[7-i]=temp;
	}
}

int ZigBeeClass::bind(uint16_t addressname){
	bind(address(),macAddress(),macAddress(addressname));
}

int ZigBeeClass::unbind(uint16_t addressname){
	unbind(address(),macAddress(),macAddress(addressname));
}

int ZigBeeClass::bind(uint16_t addressname, uint64_t sourceMac,uint64_t destinationMac){
	mac_t srcAddress,dstAddress;
	dstAddress.num64=destinationMac;
	reversemac(dstAddress.num);
	srcAddress.num64=sourceMac;
	reversemac(srcAddress.num);
	return zdoRequestBind(addressname, srcAddress.num, DEFAULT_ENDPOINT, INFO_MESSAGE_CLUSTER, DESTINATION_ADDRESS_MODE_LONG, dstAddress.num, DEFAULT_ENDPOINT, BIND);
}

int ZigBeeClass::unbind(uint16_t addressname, uint64_t sourceMac,uint64_t destinationMac){
	mac_t srcAddress,dstAddress;
	dstAddress.num64=destinationMac;
	reversemac(dstAddress.num);
	srcAddress.num64=sourceMac;
	reversemac(srcAddress.num);
	return zdoRequestBind(addressname, srcAddress.num, DEFAULT_ENDPOINT, INFO_MESSAGE_CLUSTER, DESTINATION_ADDRESS_MODE_LONG, dstAddress.num, DEFAULT_ENDPOINT, UNBIND);
}

int ZigBeeClass::bind(uint16_t addressname, uint8_t srcEndpoint, uint64_t sourceMac, uint8_t destinationEndpoint, uint64_t destinationMac, uint16_t cluster){
	mac_t srcAddress,dstAddress;
	dstAddress.num64=destinationMac;
	reversemac(dstAddress.num);
	srcAddress.num64=sourceMac;
	reversemac(srcAddress.num);
	return zdoRequestBind(addressname, srcAddress.num, srcEndpoint, INFO_MESSAGE_CLUSTER, DESTINATION_ADDRESS_MODE_LONG, dstAddress.num, destinationEndpoint, BIND);
}

int ZigBeeClass::unbind(uint16_t addressname, uint8_t srcEndpoint, uint64_t sourceMac, uint8_t destinationEndpoint, uint64_t destinationMac, uint16_t cluster){
	mac_t srcAddress,dstAddress;
	dstAddress.num64=destinationMac;
	reversemac(dstAddress.num);
	srcAddress.num64=sourceMac;
	reversemac(srcAddress.num);
	return zdoRequestBind(addressname, srcAddress.num, srcEndpoint, INFO_MESSAGE_CLUSTER, DESTINATION_ADDRESS_MODE_LONG, dstAddress.num, destinationEndpoint, UNBIND);
}

int ZigBeeClass::bindGroup(uint16_t addressname, uint64_t sourceMac, uint16_t groupname){
	mac_t srcAddress,groupAddress;
	groupAddress.num[0]=LSB(groupname);
	groupAddress.num[1]=MSB(groupname);
	srcAddress.num64=sourceMac;
	reversemac(srcAddress.num);
	return zdoRequestBind(addressname, srcAddress.num, DEFAULT_ENDPOINT, INFO_MESSAGE_CLUSTER, DESTINATION_ADDRESS_MODE_GROUP, groupAddress.num, DEFAULT_ENDPOINT, BIND);

}

int ZigBeeClass::unbindGroup(uint16_t addressname, uint64_t sourceMac, uint16_t groupname){
	mac_t srcAddress,groupAddress;
	groupAddress.num[0]=LSB(groupname);
	groupAddress.num[1]=MSB(groupname);
	srcAddress.num64=sourceMac;
	reversemac(srcAddress.num);
	return zdoRequestBind(addressname, srcAddress.num, DEFAULT_ENDPOINT, INFO_MESSAGE_CLUSTER, DESTINATION_ADDRESS_MODE_GROUP, groupAddress.num, DEFAULT_ENDPOINT, UNBIND);
}


/********************** SET MODULE CONFIGURATIONS ****************************/
void ZigBeeClass::operatingRegion(uint16_t region){
	config.operatingRegion=region;
}
void ZigBeeClass::channelMask(uint32_t _channelMask){
	config.channelMask=_channelMask;
}  
void ZigBeeClass::panId(uint16_t _panId){
	config.panId=_panId;
}
void ZigBeeClass::endDevicePollRate(uint16_t endDevicePollRate){
	config.endDevicePollRate=endDevicePollRate;
}
void ZigBeeClass::startupOptions(uint8_t _startupOptions){
	config.startupOptions=_startupOptions;
}
void ZigBeeClass::securityMode(uint8_t _securityMode){
	config.securityMode=_securityMode;
}
void ZigBeeClass::securityKey(char* _securityKey){
	config.securityKey=(uint8_t *)_securityKey;
}
void ZigBeeClass::deviceType(uint8_t _deviceType){
	config.deviceType=_deviceType;
}




/******************************** SYSTEM OPERATIONS *****************************************/



int ZigBeeClass::time(uint32_t clock){
	return sysSetTime(clock);
}

uint32_t ZigBeeClass::time(){
	if ( sysGetTime()==MODULE_SUCCESS ){
		return((uint32_t) SYS_TIME_MSB())*65536+(uint16_t)SYS_TIME_LSB();
	}else{
		return 0;
	}
}

///*
uint64_t ZigBeeClass::macAddress(){
	result=zbGetDeviceInfo(DIP_MAC_ADDRESS);
	if (result != MODULE_SUCCESS) return 0;
	mac_t a;
	for (int i =0 ; i<8; i++){
		a.num[i]=zmBuf[(7-i)+SRSP_DIP_VALUE_FIELD];
	}
	return a.num64;
}
//*/
/*
MACAddress ZigBeeClass::macAddress(){
	result=zbGetDeviceInfo(DIP_MAC_ADDRESS);
	if (result != MODULE_SUCCESS) return MACADDR_NONE;
	uint8_t i = SRSP_DIP_VALUE_FIELD;
	MACAddress macAddr= MACAddress(zmBuf[i++],zmBuf[i++],zmBuf[i++],zmBuf[i++],zmBuf[i++],zmBuf[i++],zmBuf[i++],zmBuf[i++]);
	return macAddr;
}
*/

uint64_t ZigBeeClass::macAddress(uint8_t addresstype){
	mac_t a;
	if(addresstype==SELF || addresstype==PARENT){
		if(addresstype==SELF)
			result=zbGetDeviceInfo(DIP_MAC_ADDRESS);
		else
			result=zbGetDeviceInfo(DIP_PARENT_MAC_ADDRESS);		
		if (result != MODULE_SUCCESS) return 0;		
		for (int i =0 ; i<8; i++)
			a.num[i]=zmBuf[(7-i)+SRSP_DIP_VALUE_FIELD];
	}else if(addresstype==READ){
		for (int i =0 ; i<8; i++)
			a.num[i]=read(1);
	}else if(addresstype==FROM){
		return receivedMac;
	}else{
		return 0;
	}
	return a.num64;
}


uint64_t ZigBeeClass::macAddress(uint16_t address){
	result = zdoRequestIeeeAddress(address, SINGLE_DEVICE_RESPONSE, 0);
	if (result != MODULE_SUCCESS) return 0;
	mac_t a;
	uint8_t startField=SRSP_PAYLOAD_START+1;
	for (int i =0 ; i<8; i++){
		a.num[i]=zmBuf[(7-i)+startField];
	}
	return a.num64;
}

uint16_t ZigBeeClass::address(){
    result = zbGetDeviceInfo(DIP_SHORT_ADDRESS);
    if (result != MODULE_SUCCESS) return 0xFFFF;
	return (CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD] , zmBuf[SRSP_DIP_VALUE_FIELD+1]));
}

uint16_t ZigBeeClass::address(uint8_t addresstype){
	if (addresstype==SELF){
		result = zbGetDeviceInfo(DIP_SHORT_ADDRESS);
		if (result != MODULE_SUCCESS) return 0xFFFF;
		return (CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD] , zmBuf[SRSP_DIP_VALUE_FIELD+1]));
	} else if (addresstype==PARENT){
		result = zbGetDeviceInfo(DIP_PARENT_SHORT_ADDRESS);
		if (result != MODULE_SUCCESS) return 0xFFFF;
		return (CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD] , zmBuf[SRSP_DIP_VALUE_FIELD+1]));
	} else if(addresstype==FROM){
		return receivedFromAddress;
	} else if(addresstype==TO){
		return receivedToAddress;
	} else if(addresstype==READ){
		result = read(2);
	}
}



uint16_t ZigBeeClass::address(uint64_t macAddr){
	mac_t a;
	a.num64=macAddr;
	reversemac(a.num);
	uint8_t startField;
	result=zdoNetworkAddressRequest(a.num, SINGLE_DEVICE_RESPONSE, 0);
	startField=SRSP_PAYLOAD_START+ZDO_IEEE_ADDR_RSP_SHORT_ADDRESS_FIELD_START;
    if (result != MODULE_SUCCESS) return 0xFFFF;
	return (CONVERT_TO_INT(zmBuf[startField] , zmBuf[startField+1]));
}


uint8_t ZigBeeClass::lqi(){
	return receivedLqi;
}

uint16_t ZigBeeClass::panId(){
    result = zbGetDeviceInfo(DIP_PANID);
    if (result != MODULE_SUCCESS)
		return 0xFFFF;
	else
		return (CONVERT_TO_INT(zmBuf[SRSP_DIP_VALUE_FIELD], zmBuf[SRSP_DIP_VALUE_FIELD+1]));
}

uint8_t ZigBeeClass::channel(){
    result = zbGetDeviceInfo(DIP_CHANNEL);
    if (result != MODULE_SUCCESS)
		return 0xFF;
	else
		return zmBuf[SRSP_DIP_VALUE_FIELD];
}

uint8_t ZigBeeClass::state(){
    result = zbGetDeviceInfo(DIP_STATE);
    if (result != MODULE_SUCCESS)
		return 0xFF;
	else
		return zmBuf[SRSP_DIP_VALUE_FIELD];
}

uint8_t ZigBeeClass::endpoint(){
	return DEFAULT_ENDPOINT;
}

uint8_t ZigBeeClass::endpoint(uint8_t addresstype){
	if(addresstype==TO){
		return receivedToEndpoint;
	}else if(addresstype==FROM){
		return receivedFromEndpoint;
	}else if (addresstype==READ){
		return read(1);
	}
}

uint16_t ZigBeeClass::cluster(){
	return INFO_MESSAGE_CLUSTER;
}

uint16_t ZigBeeClass::cluster(uint8_t type){
	if (type==READ)
		return receivedClusterId;
}

/*------------------------------------------------------------- MESSAGE CLASS --------------------------------------------------------*/



void ZigBeeClass::ackMode(uint8_t ack){
	afSetAckMode(ack);
}

void ZigBeeClass::flush(){
	index=0;
}

int ZigBeeClass::available(){
	return receivedLength-index;
}

int ZigBeeClass::peek(){
	if(index<MAX_MESSAGE_SIZE)
		return buffer[index];
	else{
		printf("Error: Buffer exceeds %d",MAX_MESSAGE_SIZE);
	}
}

size_t ZigBeeClass::write(uint8_t data){
	if(index<MAX_MESSAGE_SIZE)
		buffer[index++]=data;
	else{
		printf("Error: Buffer exceeds %d",MAX_MESSAGE_SIZE);
	}
}

size_t ZigBeeClass::write(uint16_t val){
	mac_t mac;
	mac.num64=(uint64_t)val;
	for(int i=0;i<2;i++)
	write(mac.num[i]);
}

size_t ZigBeeClass::write(uint32_t val){
	mac_t mac;
	mac.num64=(uint64_t)val;
	for(int i=0;i<4;i++)
	write(mac.num[i]);
}

size_t ZigBeeClass::write(uint64_t val){
	mac_t mac;
	mac.num64=val;
	for(int i=0;i<8;i++)
	write(mac.num[i]);
}



size_t ZigBeeClass::write(const uint8_t* buf, size_t size){
	for(int i=0;i<size;i++){
		write(buf[i]);
	}
}

size_t ZigBeeClass::write(char *buffer) {
    //
    //this function assumes a well formatted string (char array) has been passed in
    //
    return write((uint8_t*)buffer, strlen(buffer));
    
}

int ZigBeeClass::read(){
	if(index<MAX_MESSAGE_SIZE)
		return buffer[index++];
	else{
		printf("Error: Buffer exceeds %d",MAX_MESSAGE_SIZE);
	}
}

int ZigBeeClass::read(unsigned char* buffer, size_t size){
	int i;
	for(i=0;i<size;i++){
		buffer[i]=read();
	}
	buffer[i]='\0';
}


uint64_t ZigBeeClass::read(uint16_t numbytes){
	mac_t mac;
	for(int i=0;i<8;i++){
		mac.num[i]=(i<(numbytes&0x000F))?read():0;
	}
	return mac.num64;
}

void ZigBeeClass::printTo(Print& p, uint16_t datatype){
	printTo(p, datatype, DEC);
}

void ZigBeeClass::printTo(Print& p, uint16_t datatype, int format){
	if(datatype<CHAR)
		printTo(p,CHAR,datatype,DEC,NULL);
	else if(datatype==MAC)
		printTo(p,MAC,8,HEX,':');
	else
		printTo(p,datatype,1,format,NULL);
}

void ZigBeeClass::printTo(Print& p, uint16_t datatype, uint8_t count, int format, char separator){
	for (int i=0;i<count;i++){
		if(datatype==CHAR){
			p.print((char) read());
		}else if (datatype==UINT8 || datatype==MAC){
			if(peek(1)<format && datatype==MAC) p.print("0");
			p.print((uint8_t) read(1),format);
		}else if (datatype==UINT16){
			p.print((uint16_t) read(2),format);
		}else if (datatype==UINT32){
			p.print((uint32_t) read(4),format);
		}else if (datatype==INT8){
			p.print((int8_t) read(1),format);
		}else if (datatype==INT16){
			p.print((int16_t) read(2),format);
		}else if (datatype==INT32)
			p.print((int32_t) read(4),format);
		if(i<count-1) p.print(separator);
	}
}

void ZigBeeClass::printlnTo(Print& p, uint16_t datatype, uint8_t count, int format, char separator){
	printTo(p,  datatype,  count,  format,  separator);
	p.println();
}

void ZigBeeClass::printlnTo(Print& p, uint16_t datatype){
	printTo(p, datatype, DEC);
	p.println();
}
void ZigBeeClass::printlnTo(Print& p, uint16_t datatype, int format){
	printTo(p, datatype, format);
	p.println();
}
uint64_t ZigBeeClass::peek(uint16_t numbytes){
	uint64_t readvalue =read(numbytes);
	index-=(0x000F & numbytes);
	return readvalue;
}

uint8_t ZigBeeClass::appAdd(){return ++appCount;}
uint8_t ZigBeeClass::appNum(){return appCount;}
uint8_t ZigBeeClass::appDelete(){return --appCount;}




ZigBeeClass ZigBee;