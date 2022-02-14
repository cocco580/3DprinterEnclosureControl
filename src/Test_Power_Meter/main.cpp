

#include <Arduino.h>
#include <TrueRMS.h>
#include "config.hpp"

#define PWRLOGPERIOD 40    // loop period time in us. In this case 2.0ms

int pointNumeber = 0;

void setup() 
{
	Serial.begin(115200);
	pinMode(AC_VOLTAGE_PIN, INPUT);
	pinMode(AC_CURRENT_PIN, INPUT);
}

void loop()
{
	static unsigned long timeStamp = 0;
	static unsigned long deltaT = 0;
	VacPV = 0;
	AacPV = 0;
	pointNumeber = 0;
	deltaT = millis() - timeStamp;
	timeStamp = millis();
	while (millis() < (timeStamp + PWRLOGPERIOD) ){ //misura i valori in continuo per 20ms un ciclo intero di sinusoidale
		VacPV += pow(analogRead(AC_VOLTAGE_PIN)+AC_VOLTAGE_AC_COUPLING,2);
		AacPV += pow(analogRead(AC_CURRENT_PIN)+AC_CURRENT_AC_COUPLING,2);
		pointNumeber++;
	}
	VacPV = sqrt((1.0F/(float)pointNumeber)*VacPV);
	AacPV = sqrt((1.0F/(float)pointNumeber)*AacPV);
	VacPV = VacPV * AC_VOLTAGE_FACTOR + AC_VOLTAGE_OFFSET; //Vrms
	AacPV = AacPV * AC_CURRENT_FACTOR + AC_CURRENT_OFFSET; //Arms
	ACinstantPWR = VacPV * AacPV; //W
	ACenergy += ACinstantPWR * (float)deltaT/3600000.0F; //Wh


	Serial.print(VacPV,0);
	Serial.print("V | ");
	Serial.print(AacPV,2);
	Serial.print("A | ");
	Serial.print(pointNumeber);
	Serial.print("# | ");
	Serial.print(ACinstantPWR,0);
	Serial.print("W | ");
	Serial.print(ACenergy,2);
	Serial.println("Wh");
	delay(500);
}