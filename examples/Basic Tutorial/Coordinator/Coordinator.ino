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
        //uint8_t countReceived=ZigBee.read();
        //char stringval[16];
        //ZigBee.read(stringval,12);
        //Serial.print("Received Counter=");
        //Serial.println(countReceived);
        //Serial.println(stringval);
        ZigBee.printlnTo(Serial,UINT8);
        ZigBee.printlnTo(Serial,12);
      }
    }
  } 
  else {
    Serial.print("Disconnected...Restarting...");
    printNetwork(ZigBee.begin(COORDINATOR));
  }
}















