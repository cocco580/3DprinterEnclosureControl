#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 DisplayPV(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 DisplayFAN(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{   
    Serial.begin(9600);
    //avvio dei display
    if(!DisplayPV.begin(SSD1306_SWITCHCAPVCC,0x3C)) {
        Serial.println(F("Begin display PV failed"));
        for(;;); // Don't proceed, loop forever
    }
    DisplayPV.clearDisplay();
    DisplayPV.setTextColor(WHITE);
    DisplayPV.setTextSize(2);
    DisplayPV.setCursor(0,0);
    DisplayPV.println("Test OLED PV");
    DisplayPV.display();
    delay(5000);
    DisplayPV.clearDisplay();
    DisplayPV.fillRect(0,0,128,63,WHITE);
    DisplayPV.display();
    Serial.println("Begin display PV");

    if(!DisplayFAN.begin(SSD1306_SWITCHCAPVCC,0x3D)) {
        Serial.println(F("Begin display PV failed"));
        for(;;); // Don't proceed, loop forever
    }
    DisplayFAN.clearDisplay();
    DisplayFAN.setTextColor(WHITE);
    DisplayFAN.setTextSize(2);
    DisplayFAN.setCursor(0,0);
    DisplayFAN.println("Test OLED FAN");
    DisplayFAN.display();
    delay(5000);
    DisplayFAN.clearDisplay();
    DisplayFAN.fillRect(0,0,128,63,WHITE);
    DisplayFAN.display();
    Serial.println("Begin display FAN");
}

void loop()
{

}