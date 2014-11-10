#include <ZBooster.h>
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
  ZBooster.begin();
  Serial.begin(9600);
  Serial.print("Coordinator: Initializing Network...");
  printNetwork(ZigBee.begin(COORDINATOR));
}

#define TOGGLE_COMMAND (uint8_t) 0x02

void light_toggle(){
  digitalWrite(ZBOOSTER_RGBLED_RED,!digitalRead(ZBOOSTER_RGBLED_RED));
  digitalWrite(ZBOOSTER_RGBLED_GREEN,!digitalRead(ZBOOSTER_RGBLED_GREEN));
  digitalWrite(ZBOOSTER_RGBLED_BLUE,!digitalRead(ZBOOSTER_RGBLED_BLUE));
}

void loop()
{
  if(ZigBee.connected()){
    if(ZigBee.receive()){
      if (ZigBee.received(TYPE)==DEVICE_ANNOUNCE){
        Serial.print("Device Announce From: 0x");
        Serial.println(ZigBee.address(FROM),HEX);
      }
      else if (ZigBee.received(TYPE)==INCOMING_DATA){
        if(ZigBee.read()==TOGGLE_COMMAND){
          light_toggle();
        }
      }
    }
  }  
  else{
    Serial.print("Disconnected...Restarting...");
    printNetwork(ZigBee.begin(COORDINATOR));
  }
}







