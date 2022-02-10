#ifndef TFT_hpp
#define TFT_hpp

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS   8
#define TFT_RST  9
#define TFT_DC   10
#define TFT_MOSI 11
#define TFT_SCK  13

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

void TFT_StaticLayout(void);
void TFT_PrintTemperaturePV(int);
void TFT_PrintHumidityPV(int);
void TFT_PrintFANbar(int);
void TFT_PrintSmoke(int);
void TFT_PrintVoltage(int);
void TFT_PrintCurrent(int);
void TFT_PrintPower(int);
void TFT_PrintEnergy(int);
void TFT_PrintMode(int);
void TFT_PrintSetpoitTemperature(int);
void TFT_PrintSetpoitPercentage(int);


#endif