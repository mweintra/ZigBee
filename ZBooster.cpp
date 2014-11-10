#include "ZBooster.h"




ZBoosterClass::ZBoosterClass(){

}

void ZBoosterClass::begin(){
  pinMode(ZBOOSTER_RGBLED_RED,OUTPUT);
  pinMode(ZBOOSTER_RGBLED_GREEN,OUTPUT);
  pinMode(ZBOOSTER_RGBLED_BLUE,OUTPUT);
  pinMode(ZBOOSTER_BUTTON_SWITCH2,INPUT_PULLUP);
  
  analogWrite(ZBOOSTER_RGBLED_RED,255);
  analogWrite(ZBOOSTER_RGBLED_GREEN,255);
  analogWrite(ZBOOSTER_RGBLED_BLUE,255);
}

ZBoosterClass ZBooster;