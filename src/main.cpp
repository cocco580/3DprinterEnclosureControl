#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "definizioni.h"

//dichiarazione oggetti
Adafruit_SSD1306 DisplayPV(PIN_WIRE_SDA);
Adafruit_SSD1306 DisplayFAN(PIN_WIRE_SDA);
Adafruit_BME280 bme;


void setup() {
  // Test display PV (128x64px)
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
  delay(1000);

  // Test display FAN (128x64)
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
  delay(1000);
  
  //inizializzaione BME280
  bme.begin(0x76);

  //setup analog input
  pinMode(PotSPPin, INPUT);
  pinMode(VacPVPin, INPUT);
  pinMode(AacPVPin, INPUT);
  //setup digital input
  pinMode(BottonModePin, INPUT);
  //setup digital output
  pinMode(FanInCtrlPin, OUTPUT);
  digitalWrite(FanInCtrlPin,HIGH);
  pinMode(FanOutCtrlPin, OUTPUT);
  digitalWrite(FanOutCtrlPin,HIGH);
  pinMode(AlarmPin, OUTPUT);
  digitalWrite(AlarmPin,HIGH);

  //recupero parametri di processo precedentemente impostati e scritti sulla EEPROM
  EEPROM.get(0, kiFactor);
  EEPROM.get(4, SmokeTH);

  //inizializzazione porta seriale
  Serial.begin(9600);
}

void loop() {
  
  if (Serial.available() > 0) {
    settingProcedure(); //eventuale impostazioni parametri da seriale
  }
  getPV(); //raccolta dati
  Alarm = (SmokePV < SmokeTH) ? false : true; //dà l'allarme se il sensore rilevamento fumo supera la soglia
  alarmSequence(Alarm);
  int PWMvalue = 0;
  if(BottonMode){
    PWMvalue = controlIntegrator(TempSP, TempPV, kiFactor*(-1)); //ki negativo = raffreddamento
  }
  else{
    PWMvalue = map(PotSPValue,0,1023,0,255);
  }
  analogWrite(FanInCtrlPin, PWMvalue);
  analogWrite(FanOutCtrlPin, PWMvalue);
  printDisplayPV();
  printDisplayFAN(PWMvalue);
  //delay(5);
  
}

//sequenza di avvio e segnalazione allarme
void alarmSequence(bool StatoAllarme) {
  if (StatoAllarme){
    digitalWrite(AlarmPin, LOW); //attiva il relè
  }
  else{
    digitalWrite(AlarmPin, HIGH);
  }
  
}

//raccolta di tutti i dati provenienti dei sensori di campo
void getPV() {
  //variabili
  static unsigned long StopTime = 0;
  static unsigned long PreviusTime = 0;
  
  //da pannello frontale
  PotSPValue = analogRead(PotSPPin);
  if (digitalRead(BottonModePin)){ BottonMode = !BottonMode; }
  
  //sensori interni alla cabina di stampa
  TempSP = map(PotSPValue,0,1023,20,60);
  TempPV = bme.readTemperature(); //in gradi celsius
  PressPV = bme.readPressure() / 100.0F; // in hPa
  HumidityPV = bme.readHumidity(); //relativa

  //misura della potenza assorbita
  VacPV = 0;
  AacPV = 0;
  StopTime = millis();
  while ((millis()-StopTime) < 20 ){ //misura i valori in continuo per 20ms un ciclo intero di sinusoidale
    VacPV = max(analogRead(VacPVPin),VacPV); //aggiorna la variabile sempre con il valore max trovato
    AacPV = max(analogRead(AacPVPin),AacPV); //aggiorna la variabile sempre con il valore max trovato
  }
  for (int i=0; i<9; i++) { //array shift left
    VacPVhistory[i] = VacPVhistory[i+1];
    AacPVhistory[i] = AacPVhistory[i+1];
  }
  VacPVhistory[9] = VacPV; //aggiungi ultimo valore raccolto allo storico
  AacPVhistory[9] = AacPV; //aggiungi ultimo valore raccolto allo storico
  //valore medio dello storico
  for (int i=0; i<10; i++){ 
    VacPV += VacPVhistory[i];
    AacPV += AacPVhistory[i];
  }
  VacPV /= 10;
  AacPV /= 10;
  //scalatura a grandenzza fisica
  VacPV = VacPV * VacPVfactor * RMSfactor; //Volt
  AacPV = AacPV * AacPVfactor * RMSfactor; //Ampere
  ACinstantPWR = VacPV * AacPV / 1000; //kW
  ACenergy += ACinstantPWR * (millis() - PreviusTime)/3600000; //kWh
  PreviusTime = millis();

  //misura fumo
  for (int i = 0; i < 10; i++){
    SmokePV = analogRead(SmokePVPin);
  }
  SmokePV = SmokePV /10; //media dei valori letti
  SmokePV = map(SmokePV,0,1023,100,10000); //trasormazione in ppm

}

//sistema di controllo base PID con solo la parte integrativa
int controlIntegrator(float Command, float Feedback, float ki) {
  static float Out = 0;
  static unsigned long PreviousTime = 0;
  float ERR = Command - Feedback; //errore come differenza tra SP e PV
  int deltaT = millis() - PreviousTime; //in millisecondi
  PreviousTime = millis();
  Out += (ki * ERR * deltaT) / 1000; //il fattore 1000 converte i millisecondi in secondi
  Out = (Out > 255) ? 255 : Out; //limite superiore
  Out = (Out < 0) ? 0 : Out; //limite inferiore
  return (int) Out;
}

//imposta il display relativo alla visualizzazione dei process value
void printDisplayPV() {
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

//imposta il display relativo alla visualizzazione del controllo delle ventole
void printDisplayFAN(int FanLevel) {
  int RectWidth = map(FanLevel,0,255,0,128);

  DisplayFAN.clearDisplay();
  DisplayFAN.setTextSize(1);
  DisplayFAN.setCursor(0,0);
  DisplayFAN.print("Mode: ");
  if (BottonMode){
    DisplayFAN.println("Auto");
    DisplayFAN.setTextSize(2);
    DisplayFAN.print("SP:");
    DisplayFAN.print(round(TempSP));
    DisplayFAN.println("C");
  }
  else{
    DisplayFAN.println("Manual");
    DisplayFAN.setTextSize(2);
    DisplayFAN.print("Speed:");
    DisplayFAN.print(round(map(FanLevel,0,255,0,100)));
    DisplayFAN.println("%");
  }
  DisplayFAN.fillRect(0,28,RectWidth,4,WHITE);
  DisplayFAN.display();
}

void settingProcedure(){

  String receivedString;
  delay(10);
  receivedString = Serial.readString();
  
  if (receivedString.startsWith("SetKI")) {
    receivedString.remove(0,5);
    kiFactor = receivedString.toFloat();
    EEPROM.put(0, kiFactor);
    Serial.print("ki factor now is: ");
    Serial.println(kiFactor);
  }
/*
  if (receivedString.startsWith("GetKI")) {
    Serial.print("ki factor is: ");
    Serial.println(kiFactor);
  }
*/
  if (receivedString.startsWith("SetSK")) {
    receivedString.remove(0,5);
    SmokeTH = receivedString.toInt();
    EEPROM.put(4, SmokeTH);
    Serial.print("Smoke TH now is: ");
    Serial.println(SmokeTH);
  }
/*
  if (receivedString.startsWith("GetSK")) {
    Serial.print("Smoke TH is: ");
    Serial.println(SmokeTH);
  }
*/
  Serial.flush();
}
