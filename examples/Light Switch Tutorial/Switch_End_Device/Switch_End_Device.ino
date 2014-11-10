#include <ZBooster.h>
#include <SPI.h>
#include <ZigBee.h>

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
  Serial.print("End Device: Joining Network...");
  printNetwork(ZigBee.begin(END_DEVICE));
}

void light_toggle(uint8_t toggleLed){

  digitalWrite(ZBOOSTER_RGBLED_RED,!digitalRead(ZBOOSTER_RGBLED_RED));
  digitalWrite(ZBOOSTER_RGBLED_GREEN,!digitalRead(ZBOOSTER_RGBLED_GREEN));
  digitalWrite(ZBOOSTER_RGBLED_BLUE,!digitalRead(ZBOOSTER_RGBLED_BLUE));
}

#define TOGGLE_COMMAND (uint8_t) 0x02

void light_toggle_request(){
  ZigBee.write(TOGGLE_COMMAND);
  if (ZigBee.send(0)==SUCCESS){
    Serial.println("Light Toggle Request Send Success");
  } 
  else{
    Serial.println("Send Fail...");
    printNetwork(ZigBee.reconnect());
  }
}

uint32_t lastDebounceTime=0;
uint32_t debounceDelay = 50;

void loop(){
  if(ZigBee.connected()){
    if(digitalRead(ZBOOSTER_BUTTON_SWITCH2)==LOW){
      if(millis()-lastDebounceTime>debounceDelay){
        light_toggle_request();
        while(digitalRead(ZBOOSTER_BUTTON_SWITCH2)==LOW);
        lastDebounceTime=millis();
      }
    }  
  }
  else{
    Serial.print("Disconnected...Restarting...");
    printNetwork(ZigBee.begin(END_DEVICE));
  }
}







