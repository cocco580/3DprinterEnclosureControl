

#include <Arduino.h>
#include <TrueRMS.h>
#include "config.hpp"

#define PWRLOGPERIOD 20    // loop period time in us. In this case 2.0ms

int pointNumeber = 0;

void setup() 
{
	Serial.begin(115200);
	pinMode(VacPVPin, INPUT);
	pinMode(AacPVPin, INPUT);
}

void loop()
{
	static unsigned long timeStamp = 0;
	VacPV = 0;
	AacPV = 0;
	pointNumeber = 0;
	timeStamp = millis();
	while (millis() < (timeStamp + PWRLOGPERIOD) ){ //misura i valori in continuo per 20ms un ciclo intero di sinusoidale
		VacPV += pow(analogRead(VacPVPin),2); 
		AacPV += pow(analogRead(AacPVPin),2);
		pointNumeber++;
	}
	VacPV = sqrt((1/pointNumeber)*VacPV);
	AacPV = sqrt((1/pointNumeber)*AacPV);
	
	Serial.print(VacPV,3);
	Serial.print(" | ");
	Serial.print(AacPV,3);
	Serial.print(" | ");
	Serial.println(pointNumeber);
	delay(500);
}