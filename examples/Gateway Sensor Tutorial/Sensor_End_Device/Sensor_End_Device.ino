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
    ZigBee.time(1415589823); // http://www.epochconverter.com/
  }
  else{
    Serial.print("Error: 0x");
    Serial.println(connectStatus,HEX);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.print("End Device: Joining Network...");
  printNetwork(ZigBee.begin(END_DEVICE));
}
uint8_t counter=0;

void loop()
{
  if(ZigBee.connected()){ // check if network joined
    ZigBee.write(counter);
    ZigBee.write(voltage());
    ZigBee.write(temperature());
    ZigBee.write(ZigBee.time());
    ZigBee.write(ZigBee.macAddress());
    if (ZigBee.send(0)==SUCCESS){ 
      Serial.print("Send Success! Counter=");
      Serial.println(counter++);
    } 
    else{
      Serial.println("Send Fail...Reconnecting...");
      printNetwork(ZigBee.reconnect());
    }
    delay(1000); // Without this you would add too much traffic
  }
  else{
    Serial.print("Disconnected...Restarting...");
    printNetwork(ZigBee.begin(END_DEVICE));
  }
}















