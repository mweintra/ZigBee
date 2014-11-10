#include <MACAddress.h>
#include <ZigBee.h>
#include <SPI.h>

void printNetwork(int connectStatus){
  if(connectStatus==SUCCESS){
    Serial.println("Success!");
    Serial.print("Connected to: PanID: 0x");
    Serial.print(ZigBee.panId(),HEX);
    Serial.print(", Channel: 0x");
    Serial.print(ZigBee.channel(),HEX);
    Serial.print(", Address: 0x");
    Serial.println(ZigBee.address(),HEX);
  }
  else{
    Serial.print("Error: 0x");
    Serial.println(connectStatus,HEX);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.print("Coordinator: Initializing Network...");
  printNetwork(ZigBee.begin(COORDINATOR));
}



void loop(){
  if(ZigBee.connected()){ // check if network joined
    if (ZigBee.receive()){
      if (ZigBee.received(TYPE)==DEVICE_ANNOUNCE){
        Serial.print("Device Announce From: 0x");
        Serial.println(ZigBee.address(FROM),HEX);
      }
      else if (ZigBee.received(TYPE)==INCOMING_DATA){
        for (int i=0;i<80;i++){
          Serial.print("-");
        }
        Serial.println();
        Serial.print("Count: ");
        ZigBee.printlnTo(Serial,UINT8);

        Serial.print("Voltage: ");
        int16_t voltage = ZigBee.read(INT16);
        Serial.print(voltage/1000);
        Serial.print(".");
        Serial.print((voltage%1000)/100);
        Serial.print((voltage%100)/10);
        Serial.print((voltage%10));
        Serial.println("V");

        Serial.print("Temperature: ");
        int16_t temperature= ZigBee.read(INT16);
        Serial.print(temperature/10);
        Serial.print(".");
        Serial.print(temperature%10);
        Serial.println('C');

        Serial.print("Date: ");
        uint32_t timeval = ZigBee.read(UINT32);
        datePrint(Serial,timeval);
        Serial.print(" ");
        timePrint(Serial,timeval);
        Serial.println();

        Serial.print("MAC Address: ");
        MACAddress macAddr = MACAddress(ZigBee.read(MAC));
        Serial.println(macAddr);   
      }
    }
  } 
  else {
    Serial.print("Disconnected...Restarting...");
    printNetwork(ZigBee.begin(COORDINATOR));
  }
}

















