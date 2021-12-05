
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.hpp"

//dichiarazione oggetti
Adafruit_SSD1306 DisplayPV(PIN_WIRE_SDA);
Adafruit_SSD1306 DisplayFAN(PIN_WIRE_SDA);

// Test display PV (128x64px)
void beginDisplayPV()
{
    
  DisplayPV.begin(SSD1306_SWITCHCAPVCC,0x3C); //indirizzo del primo display
  DisplayPV.clearDisplay();
  DisplayPV.setTextColor(WHITE);
  /* DisplayPV.setTextSize(2);
  DisplayPV.setCursor(0,0);
  DisplayPV.println("Test OLED PV");
  DisplayPV.display();
  delay(1000);
  DisplayPV.clearDisplay();
  DisplayPV.fillRect(0,0,128,63,WHITE); */
  DisplayPV.display();
}

// Test display FAN (128x64)
void beginDisplayFAN()
{
  DisplayFAN.begin(SSD1306_SWITCHCAPVCC,0x3D); //indirizzo del secondo display
  DisplayFAN.clearDisplay();
  DisplayFAN.setTextColor(WHITE);
  /* DisplayFAN.setTextSize(2);
  DisplayFAN.setCursor(0,0);
  DisplayFAN.println("Test OLED FAN");
  DisplayFAN.display();
  delay(1000);
  DisplayFAN.clearDisplay();
  DisplayFAN.fillRect(0,0,128,63,WHITE); */
  DisplayFAN.display();
}

//imposta il display relativo alla visualizzazione dei process value
void printDisplayPV(float TempPV, float HumidityPV, int SmokePV, float VacPV, float AacPV, float ACinstantPWR, float ACenergy)
{
  DisplayPV.clearDisplay();
  DisplayPV.setTextSize(2);
  DisplayPV.setCursor(0,0);
  DisplayPV.print("T:");
  DisplayPV.print(round(TempPV));
  DisplayPV.println("C");
  DisplayPV.setTextSize(1);
  DisplayPV.print("H:");
  DisplayPV.print(round(HumidityPV));
  DisplayPV.println("%");
  DisplayPV.print("Smk:");
  DisplayPV.print(round(SmokePV));
  //DisplayPV.println("ppm");
  DisplayPV.setTextSize(1);
  DisplayPV.setCursor(64,0);
  DisplayPV.print("Vac:");
  DisplayPV.print(round(VacPV));
  DisplayPV.println("V");
  DisplayPV.setCursor(64,8);
  DisplayPV.print("Aac:");
  DisplayPV.print(AacPV,2);
  DisplayPV.println("A");
  DisplayPV.setCursor(64,16);
  DisplayPV.print("PWR:");
  DisplayPV.print(round(ACinstantPWR));
  DisplayPV.println("kW");
  DisplayPV.setCursor(64,24);
  DisplayPV.print("ERG:");
  DisplayPV.print(round(ACenergy));
  DisplayPV.println("kWh");
  DisplayPV.display();
}

//imposta il display relativo alla visualizzazione del controllo delle ventole in Automatico
void printDisplayFAN_AUTO(float TempSP, int FanLevel)
{
  static int RectWidth = 0;
  RectWidth = map(FanLevel,0,255,0,128);

  DisplayFAN.clearDisplay();
  DisplayFAN.setTextSize(1);
  DisplayFAN.setCursor(0,0);
  DisplayFAN.print("Mode: ");
  DisplayFAN.println("Auto");
  DisplayFAN.setTextSize(2);
  DisplayFAN.print("SP:");
  DisplayFAN.print(round(TempSP));
  DisplayFAN.println("C");
  DisplayFAN.fillRect(0,28,RectWidth,4,WHITE);
  DisplayFAN.display();
}

//imposta il display relativo alla visualizzazione del controllo delle ventole in Manuale
void printDisplayFAN_MAN(int FanLevel)
{
  static int RectWidth = 0;
  RectWidth = map(FanLevel,0,255,0,128);

  DisplayFAN.clearDisplay();
  DisplayFAN.setTextSize(1);
  DisplayFAN.setCursor(0,0);
  DisplayFAN.print("Mode: ");
  DisplayFAN.println("Manual");
  DisplayFAN.setTextSize(2);
  DisplayFAN.print("Speed:");
  DisplayFAN.print(round(map(FanLevel,0,255,0,100)));
  DisplayFAN.println("%");
  DisplayFAN.fillRect(0,28,RectWidth,4,WHITE);
  DisplayFAN.display();
}

//imposta il display relativo alla visualizzazione del controllo delle ventole in Stop
void printDisplayFAN_STOP()
{
  //static int RectWidth = 0;
  //RectWidth = map(FanLevel,0,255,0,128);

  DisplayFAN.clearDisplay();
  DisplayFAN.setTextSize(1);
  DisplayFAN.setCursor(0,0);
  DisplayFAN.println("Mode: ");
  DisplayFAN.setTextSize(2);
  DisplayFAN.println("STOP");
  //DisplayFAN.fillRect(0,28,RectWidth,4,WHITE);
  DisplayFAN.display();
}