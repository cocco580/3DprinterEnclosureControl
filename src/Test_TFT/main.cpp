#include <Arduino.h>
#include "TFT.hpp"

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("TFT Test!");
  
  TFT_StaticLayout();
}

void loop(void) {
  Serial.println("On loop");
  TFT_PrintTemperaturePV((int) random(0,100));
  TFT_PrintHumidityPV((int) random(0,100));
  TFT_PrintSmoke((int) random(0,9999));
  TFT_PrintVoltage((int) random(190,250));
  TFT_PrintCurrent((int) random(0,99));
  TFT_PrintPower((int) random(0,999));
  TFT_PrintEnergy((int) random(0,9999));
  TFT_PrintFANbar((int) random(0,255));
  TFT_PrintMode(0);
  delay(1000);
  TFT_PrintMode(1);
  TFT_PrintSetpoitTemperature((int) random(0,99));
  delay(1000);
  TFT_PrintSetpoitPercentage((int) random(0,100));
  delay(1000);
}