#define REF2_5V_VOLTAGE_MV    2500l
#ifndef VCC2	
#define VCC2  (128 + 11)
#endif
#define TEMPERATURE_CALIBRATION_OFFSET_MV	1006 
#define TEMPERATURE_CALIBRATION_SLOPE_MV	355
#define R2MV ((REF2_5V_VOLTAGE_MV*1000)/10241)

uint16_t voltage()
{
  analogReference(INTERNAL2V5);              
  delay(1);         
  unsigned long temp = (analogRead(VCC2) * (REF2_5V_VOLTAGE_MV * 2));        // Convert raw ADC value to millivolts
  return ((uint16_t) (temp / 1024l));
}
 
uint16_t temperature()
{
  analogReference(INTERNAL2V5);               
  delay(1);     
  unsigned long temp = (analogRead(TEMPSENSOR));        // Convert raw ADC value to millivolts
  temp=temp*R2MV*10-(unsigned long) TEMPERATURE_CALIBRATION_OFFSET_MV*1000;
  return (uint16_t) (temp/TEMPERATURE_CALIBRATION_SLOPE_MV);
}


